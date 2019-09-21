open Sonos.Methods;
open Sonos.Decode;
open Js.Promise;

let device = Config.device;

let trackPosition = (~first, ~queueAt, ()) =>
  int_of_string(first) - int_of_float(queueAt) + 1 |> string_of_int;

let listTracks = (tracks: array(currentQueue)) =>
  tracks->Belt.Array.mapWithIndex((i, {artist, title}) =>
    Utils.listNumber(i) ++ Utils.artistAndTitle(~artist, ~title)
  );

let queueWithFallback = () =>
  device->getQueue()
  |> then_(queue =>
       (
         switch (Js.Types.classify(queue)) {
         | JSFalse =>
           {|{ "returned": "0", "total": "0", "items": [] }|}
           |> Json.parseOrRaise
         | _ => queue
         }
       )
       |> currentQueueResponse
       |> resolve
     );

/* CLI style */
let clear = () =>
  device->Sonos.Methods.flush()
  |> then_(_ => `Ok("Cleared queue") |> resolve)
  |> catch(_ => `Failed("Failed to clear queue") |> resolve);

let current = () =>
  queueWithFallback()
  |> then_(({items}) =>
       Services.getCurrentTrack()
       |> then_(({queuePosition}) => {
            let numberOfTracks = items->Belt.Array.length;

            let message =
              switch (numberOfTracks, queuePosition) {
              | (0, _) => Messages.emptyQueue
              | (nt, qp) when int_of_float(qp) == nt => Messages.emptyQueue
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

            `Ok(message) |> resolve;
          })
       |> catch(_ => `Failed("Failed to get current queue") |> resolve)
     )
  |> catch(_ => `Failed("Failed to get current queue") |> resolve);

let full = () =>
  queueWithFallback()
  |> then_(({items}) => {
       let message =
         switch (items->Belt.Array.length) {
         | 0 => Messages.emptyQueue
         | _ =>
           let tracks = items->listTracks->Utils.joinWithNewline;
           "*Here's the full queue*\n" ++ tracks;
         };

       `Ok(message) |> resolve;
     })
  |> catch(_ => `Failed("Failed to get full queue") |> resolve);

module Exists = {
  type t =
    | InQueue
    | NotInQueue;

  let inQueue = trackUri => {
    queueWithFallback()
    |> then_(({items}) => {
         let existsInQueue =
           items
           ->Belt.Array.map(item => Utils.sonosUriToSpotifyUri(item.uri))
           ->Belt.Array.some(uri => uri == trackUri);

         resolve(existsInQueue ? InQueue : NotInQueue);
       });
  };
};

let last = track => {
  let parsedTrack = Utils.parsedTrack(track);

  Exists.inQueue(parsedTrack)
  |> then_((existsInQueue: Exists.t) =>
       switch (existsInQueue) {
       | InQueue => resolve(`Ok(Messages.trackExistsInQueue))
       | NotInQueue =>
         device->queueAsLast(parsedTrack)
         |> then_(queuedTrack =>
              Services.getCurrentTrack()
              |> then_(({queuePosition}: currentTrackResponse) => {
                   let {firstTrackNumberEnqueued}: queueResponse =
                     queuedTrack->queueResponse;

                   let message =
                     "Sweet! Your track is number *"
                     ++ trackPosition(
                          ~first=firstTrackNumberEnqueued,
                          ~queueAt=queuePosition,
                          (),
                        )
                     ++ "* in the queue :musical_note:";

                   `Ok(message) |> resolve;
                 })
            )
         |> catch(_ => `Failed("Failed to queue track") |> resolve)
       }
     );
};

let next = track => {
  let parsedTrack = Utils.parsedTrack(track);

  Exists.inQueue(parsedTrack)
  |> then_((existsInQueue: Exists.t) =>
       switch (existsInQueue) {
       | InQueue => resolve(`Ok(Messages.trackExistsInQueue))
       | NotInQueue =>
         Services.getCurrentTrack()
         |> then_(({position, queuePosition}) =>
              device->queue(parsedTrack, int_of_float(queuePosition) + 1)
              |> then_(() => {
                   let message =
                     switch (position, queuePosition) {
                     | (0., 0.) => "Your track will play right now"
                     | _ => "Your track will play right after the current"
                     };

                   `Ok(message) |> resolve;
                 })
              |> catch(_ => `Failed("Failed to queue track") |> resolve)
            )
       }
     );
};

type multiple = {
  exists: list(string),
  newTracks: list(string),
};

let multiple = tracks => {
  let parsedTracks = tracks->Belt.Array.map(Utils.parsedTrack);
  let exists = parsedTracks->Belt.Array.map(track => Exists.inQueue(track));

  all(exists)
  |> then_(allTracks => {
       let (inQueue, notInQueue) =
         allTracks->Belt.Array.reduceWithIndex(
           (0, 0), ((inQueue, notInQueue), curr, i) =>
           switch (curr) {
           | Exists.InQueue => (inQueue + 1, notInQueue)
           | NotInQueue =>
             last(parsedTracks[i]) |> ignore;

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

       resolve(`Ok(message));
     });
};
