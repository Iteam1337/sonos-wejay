open Js.Promise;
open Tablecloth;

let device = Config.device;

let trackPosition = (~first, ~queueAt, ()) =>
  int_of_string(first) - int_of_float(queueAt) + 1 |> string_of_int;

let listTracks = (tracks: array(Sonos.Decode.CurrentQueue.queueItem)) =>
  tracks->Array.mapWithIndex(~f=(i, {artist, title}) =>
    Utils.listNumber(i) ++ Utils.artistAndTitle(~artist, ~title)
  );

module WithFallback = {
  type t =
    | Queue(Sonos.Decode.CurrentQueue.t)
    | NoTracks;

  let make = () => {
    let%Async queue = device->Sonos.Methods.Queue.get();

    switch (Js.Types.classify(queue)) {
    | JSFalse => NoTracks |> resolve
    | _ => Queue(Sonos.Decode.CurrentQueue.make(queue)) |> resolve
    };
  };
};

/* CLI style */
let clear = () => {
  let%Async _ = device->Sonos.Methods.Queue.clear();

  Slack.Msg.make([
    `Section(
      "I have cleared the queue for you. Add some new tracks! :dusty_stick:",
    ),
  ]);
};

let handleRemoveArgs = args => {
  switch (args) {
  | "" => (1, 1)
  | args =>
    let input = args |> Js.String.split(" ");
    let numberOfTracks =
      input
      ->Belt.Array.get(0)
      ->Belt.Option.getWithDefault("1")
      ->int_of_string;
    let index =
      input
      ->Belt.Array.get(1)
      ->Belt.Option.getWithDefault("1")
      ->int_of_string;
    (numberOfTracks, index);
  };
};

let removeMultipleTracks = args => {
  let (numberOfTracks, index) = handleRemoveArgs(args);

  Sonos.Methods.Queue.removeMultipleTracks(device, index, numberOfTracks)
  |> then_(_ => {
       let message =
         switch (index, numberOfTracks) {
         | (_, 1) => "I removed it for you!"
         | _ => "I removed them for you!"
         };

       Slack.Msg.make([`Section(message)]);
     });
};

let current = () => {
  let%Async queue = WithFallback.make();

  switch (queue) {
  | Queue({items}) =>
    let%Async {queuePosition} = Services.getCurrentTrack();
    let numberOfTracks = items->Belt.Array.length;

    let message =
      switch (numberOfTracks, queuePosition) {
      | (0, _) => Message.emptyQueue
      | (nt, qp) when int_of_float(qp) == nt => Message.emptyQueue
      | _ =>
        let tracks =
          items
          ->Belt.Array.slice(
              ~offset=queuePosition |> int_of_float,
              ~len=Belt.Array.length(items),
            )
          ->listTracks
          ->Utils.joinWithNewline;

        "*Upcoming tracks*\n" ++ tracks;
      };

    Slack.Msg.make([`Section(message)]);
  | NoTracks => Slack.Msg.make([`Section(Message.emptyQueue)])
  };
};

let full = () => {
  let%Async queue = WithFallback.make();

  switch (queue) {
  | Queue({items}) =>
    let message =
      switch (items->Belt.Array.length) {
      | 0 => Message.emptyQueue
      | _ =>
        let tracks = items->listTracks->Utils.joinWithNewline;
        "*Here's the full queue*\n" ++ tracks;
      };

    Slack.Msg.make([`Section(message)]);
  | NoTracks => Slack.Msg.make([`Section(Message.emptyQueue)])
  };
};

module Exists = {
  type t =
    | InQueue
    | NotInQueue;

  let inQueue = track => {
    switch (track) {
    | Utils.Parse.Playlist(_) => resolve(NotInQueue)
    | Track(uri) =>
      uri
      ->SpotifyUtils.trackId
      ->Belt.Option.getWithDefault("")
      ->Spotify.Track.make
      |> then_((spotify: Spotify.Track.t) =>
           WithFallback.make()
           |> then_((queue: WithFallback.t) => {
                switch (queue) {
                | Queue({items}) =>
                  /* Only checking URI is not enough, so we do a
                   * naive artist/track name match as well. Sonos
                   * sometimes uses a different track URI
                   * than what was pasted in Wejay because
                   * of different regions or markets */
                  let existsInQueue =
                    items->Belt.Array.some(item =>
                      Utils.Sonos.toSpotifyUri(Some(item.uri)) === uri
                      || item.artist === spotify.artist
                      && item.title === spotify.name
                    );

                  resolve(existsInQueue ? InQueue : NotInQueue);
                | NoTracks => resolve(NotInQueue)
                }
              })
         )
    };
  };
};

module AsLastTrack = {
  let queue = track => {
    switch (track) {
    | Utils.Parse.Track(uri) =>
      device->Sonos.Methods.Queue.asLast(uri)
      |> then_(queuedTrack =>
           Services.getCurrentTrack()
           |> then_(({queuePosition}: Sonos.Decode.CurrentTrack.t) => {
                let {firstTrackNumberEnqueued}: Sonos.Decode.QueueTrack.t =
                  queuedTrack->Sonos.Decode.QueueTrack.make;

                let message =
                  "Sweet! Your track is number *"
                  ++ trackPosition(
                       ~first=firstTrackNumberEnqueued,
                       ~queueAt=queuePosition,
                       (),
                     )
                  ++ "* in the queue :musical_note:";

                Slack.Msg.make([`Section(message)]);
              })
         )
    | Playlist(uri) =>
      switch (SpotifyUtils.trackId(uri)) {
      | Some(id) =>
        Spotify.Playlist.make(id)
        |> then_(({owner}: Spotify.Playlist.t) =>
             device->Sonos.Methods.Queue.asLast(
               {j|spotify:user:$owner:playlist:$id|j},
             )
             |> then_(_ =>
                  Slack.Msg.make([`Section("Playlist has been queued")])
                )
           )
      | None => Slack.Msg.make([`Section("Nothing queued, ID was invalid")])
      }
    };
  };

  let make = (track, ~skipExists=false, ()) => {
    switch (track) {
    | "" =>
      Slack.Msg.make([
        `Section(
          "You forgot to tell me what I should add to the queue\n*Example:* `q spotify:track:4fK6E2UywZTJIa5kWnCD6x`",
        ),
      ])
    | track =>
      let parsedTrack = Utils.Parse.make(track);

      switch (skipExists, parsedTrack) {
      | (false, Playlist(_))
      | (true, _) => queue(parsedTrack)
      | (false, Track(_)) =>
        Exists.inQueue(parsedTrack)
        |> then_((existsInQueue: Exists.t) =>
             switch (existsInQueue) {
             | InQueue =>
               Slack.Msg.make([`Section(Message.trackExistsInQueue)])
             | NotInQueue =>
               queue(parsedTrack)
               |> catch(_ => Error("Failed to queue track") |> resolve)
             }
           )
      };
    };
  };
};

let next = track => {
  let parsedTrack = Utils.Parse.make(track);

  switch (parsedTrack) {
  | Utils.Parse.Playlist(uri)
  | Track(uri) =>
    Exists.inQueue(parsedTrack)
    |> then_((existsInQueue: Exists.t) =>
         switch (existsInQueue) {
         | InQueue => Slack.Msg.make([`Section(Message.trackExistsInQueue)])
         | NotInQueue =>
           Services.getCurrentTrack()
           |> then_(({position, queuePosition}: Sonos.Decode.CurrentTrack.t) =>
                device->Sonos.Methods.Queue.atPosition(
                  uri,
                  int_of_float(queuePosition) + 1,
                )
                |> then_(() => {
                     let message =
                       switch (position, queuePosition) {
                       | (0., 0.) => "Your track will play right now"
                       | _ => "Your track will play right after the current"
                       };

                     Slack.Msg.make([`Section(message)]);
                   })
                |> catch(_ => Error("Failed to queue track") |> resolve)
              )
         }
       )
  };
};

type multiple = {
  exists: list(string),
  newTracks: list(string),
};

let multiple = tracks => {
  let parsedTracks = tracks->Array.map(~f=Utils.Parse.make);
  let exists = parsedTracks->Array.map(~f=track => Exists.inQueue(track));

  all(exists)
  |> then_(allTracks => {
       let (inQueue, notInQueue) =
         allTracks->Belt.Array.reduceWithIndex(
           (0, 0), ((inQueue, notInQueue), curr, i) =>
           switch (curr) {
           | Exists.InQueue => (inQueue + 1, notInQueue)
           | NotInQueue =>
             switch (parsedTracks |> Array.get(~index=i)) {
             | Some(Playlist(uri))
             | Some(Track(uri)) =>
               AsLastTrack.make(uri, ~skipExists=true, ()) |> ignore
             | None => ()
             };

             (inQueue, notInQueue + 1);
           }
         );

       let message =
         switch (inQueue, notInQueue) {
         | (1, 0) => "Skipped *1* track that's already queued"
         | (x, 0) => {j|Skipped *$x* tracks that are already queued|j}
         | (0, 1) => "Queued *1* track"
         | (0, x) => {j|Queued *$x* tracks|j}
         | (1, 1) => {j|Queued *1* track, skipped *1* that is already queued|j}
         | (x, 1) => {j|Queued *1* track, skipped *$x* that are already queued|j}
         | (x, y) => {j|Queued *$y* tracks, skipped *$x* that are already queued|j}
         };

       Slack.Msg.make([`Section(message)]);
     });
};
