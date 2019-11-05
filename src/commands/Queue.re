open Js.Promise;

let device = Config.device;

let trackPosition = (~first, ~queueAt, ()) =>
  int_of_string(first) - int_of_float(queueAt) + 1 |> string_of_int;

let listTracks = (tracks: array(Sonos.Decode.CurrentQueue.queueItem)) =>
  tracks->Belt.Array.mapWithIndex((i, {artist, title}) =>
    Utils.listNumber(i) ++ Utils.artistAndTitle(~artist, ~title)
  );

let queueWithFallback = () =>
  device->Sonos.Methods.Queue.get()
  |> then_(queue =>
       (
         switch (Js.Types.classify(queue)) {
         | JSFalse =>
           {|{ "returned": "0", "total": "0", "items": [] }|}
           |> Json.parseOrRaise
         | _ => queue
         }
       )
       |> Sonos.Decode.CurrentQueue.make
       |> resolve
     );

/* CLI style */
let clear = () =>
  device->Sonos.Methods.Queue.clear()
  |> then_(_ =>
       `Ok(
         Slack.Block.make([
           `Section(
             "I have cleared the queue for you. Add some new tracks! :dusty_stick:",
           ),
         ]),
       )
       |> resolve
     )
  |> catch(_ => `Failed("Failed to clear queue") |> resolve);

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
       `Ok(Slack.Block.make([`Section(message)])) |> resolve;
     });
};

let current = () =>
  queueWithFallback()
  |> then_(({items}: Sonos.Decode.CurrentQueue.t) =>
       Services.getCurrentTrack()
       |> then_(({queuePosition}: Sonos.Decode.CurrentTrack.t) => {
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

            `Ok(Slack.Block.make([`Section(message)])) |> resolve;
          })
       |> catch(_ => `Failed("Failed to get current queue") |> resolve)
     )
  |> catch(_ => `Failed("Failed to get current queue") |> resolve);

let full = () =>
  queueWithFallback()
  |> then_(({items}: Sonos.Decode.CurrentQueue.t) => {
       let message =
         switch (items->Belt.Array.length) {
         | 0 => Message.emptyQueue
         | _ =>
           let tracks = items->listTracks->Utils.joinWithNewline;
           "*Here's the full queue*\n" ++ tracks;
         };

       `Ok(Slack.Block.make([`Section(message)])) |> resolve;
     })
  |> catch(_ => `Failed("Failed to get full queue") |> resolve);

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
      ->Spotify.getSpotifyTrack
      |> then_((spotify: Spotify.WejayTrack.t) =>
           queueWithFallback()
           |> then_(({items}: Sonos.Decode.CurrentQueue.t) => {
                /* Only checking URI is not enough, so we do a
                 * naive artist/track name match as well. Sonos
                 * sometimes uses a different track URI
                 * than what was pasted in Wejay because
                 * of different regions or markets */
                let existsInQueue =
                  items->Belt.Array.some(item =>
                    Utils.sonosUriToSpotifyUri(Some(item.uri)) === uri
                    || item.artist === spotify.artist
                    && item.title === spotify.name
                  );

                resolve(existsInQueue ? InQueue : NotInQueue);
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

                `Ok(Slack.Block.make([`Section(message)])) |> resolve;
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
                  `Ok(
                    Slack.Block.make([`Section("Playlist has been queued")]),
                  )
                  |> resolve
                )
           )
      | None =>
        resolve(
          `Ok(
            Slack.Block.make([`Section("Nothing queued, ID was invalid")]),
          ),
        )
      }
    };
  };

  let make = (track, ~skipExists=false, ()) => {
    switch (track) {
    | "" =>
      `Ok(
        Slack.Block.make([
          `Section(
            "You forgot to tell me what I should add to the queue\n*Example:* `q spotify:track:4fK6E2UywZTJIa5kWnCD6x`",
          ),
        ]),
      )
      |> resolve
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
               resolve(
                 `Ok(
                   Slack.Block.make([`Section(Message.trackExistsInQueue)]),
                 ),
               )
             | NotInQueue =>
               queue(parsedTrack)
               |> catch(_ => `Failed("Failed to queue track") |> resolve)
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
         | InQueue =>
           resolve(
             `Ok(Slack.Block.make([`Section(Message.trackExistsInQueue)])),
           )
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

                     `Ok(Slack.Block.make([`Section(message)])) |> resolve;
                   })
                |> catch(_ => `Failed("Failed to queue track") |> resolve)
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
  let parsedTracks = tracks->Belt.Array.map(Utils.Parse.make);
  let exists = parsedTracks->Belt.Array.map(track => Exists.inQueue(track));

  all(exists)
  |> then_(allTracks => {
       let (inQueue, notInQueue) =
         allTracks->Belt.Array.reduceWithIndex(
           (0, 0), ((inQueue, notInQueue), curr, i) =>
           switch (curr) {
           | Exists.InQueue => (inQueue + 1, notInQueue)
           | NotInQueue =>
             switch (parsedTracks[i]) {
             | Playlist(uri)
             | Track(uri) =>
               AsLastTrack.make(uri, ~skipExists=true, ()) |> ignore
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

       resolve(`Ok(Slack.Block.make([`Section(message)])));
     });
};
