open Sonos.Methods;
open Sonos.Decode;
open Js.Promise;

let device = Config.device;

let trackPosition = (~first, ~queueAt, ()) =>
  int_of_string(first) - int_of_float(queueAt) + 1 |> string_of_int;

let asLast = (~track, ~user, ~sendMessage=?, ()) => {
  let parsedTrack = Utils.parsedTrack(track);

  switch (user) {
  | Some(u) => Database.insertTrack(~uri=parsedTrack, ~user=u)
  | None => ()
  };

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

            switch (sendMessage) {
            | Some(sendMsg) => sendMsg(message)
            | _ => ()
            };

            resolve(message);
          })
     )
  |> catch(Utils.handleError("queueAsLast"));
};

let asNext = (track, user, sendMessage) => {
  let parsedTrack = Utils.parsedTrack(track);

  switch (user) {
  | Some(u) => Database.insertTrack(~uri=parsedTrack, ~user=u)
  | None => ()
  };

  Services.getCurrentTrack()
  |> then_(({position, queuePosition}) =>
       device->queue(parsedTrack, int_of_float(queuePosition) + 1)
       |> then_(() => {
            switch (position, queuePosition) {
            | (0., 0.) => sendMessage("Your track will play right now")
            | _ => sendMessage("Your track will play right after the current")
            };

            resolve(true);
          })
       |> catch(Utils.handleError("queueAsNext"))
     )
  |> ignore;
};

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

let currentQueue = sendMessage =>
  queueWithFallback()
  |> then_(({items}) =>
       Services.getCurrentTrack()
       |> then_(({queuePosition}) => {
            let numberOfTracks = items->Belt.Array.length;

            switch (numberOfTracks, queuePosition) {
            | (0, _) => sendMessage(Messages.emptyQueue)
            | (nt, qp) when int_of_float(qp) == nt =>
              sendMessage(Messages.emptyQueue)
            | _ =>
              let tracks =
                items
                ->Belt.Array.slice(
                    ~offset=queuePosition |> int_of_float,
                    ~len=Belt.Array.length(items),
                  )
                ->listTracks
                ->Utils.joinWithNewline;

              sendMessage("*Upcoming tracks*\n" ++ tracks);
            };

            queue |> resolve;
          })
       |> catch(Utils.handleError("currentQueue -> currentTrack"))
     )
  |> catch(Utils.handleError("currentQueue"))
  |> ignore;

let getFullQueue = sendMessage =>
  queueWithFallback()
  |> then_(({items}) => {
       switch (items->Belt.Array.length) {
       | 0 => sendMessage(Messages.emptyQueue)
       | _ =>
         let tracks = items->listTracks->Utils.joinWithNewline;
         sendMessage("*Here's the full queue*\n" ++ tracks);
       };

       resolve(true);
     })
  |> catch(Utils.handleError("getFullQueue"))
  |> ignore;

let clearQueue = sendMessage =>
  device->Sonos.Methods.flush()
  |> then_(value => {
       sendMessage("Cleared queue");
       value |> resolve;
     })
  |> catch(Utils.handleError("clearPlaylist"))
  |> ignore;

let addMultipleTracks = (tracks, user, sendMessage) =>
  tracks->Belt.Array.forEach(track =>
    asLast(~track, ~user, ~sendMessage, ()) |> ignore
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

let last = track => {
  let parsedTrack = Utils.parsedTrack(track);

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
  |> catch(_ => `Failed("Failed to queue track") |> resolve);
};

let next = track => {
  let parsedTrack = Utils.parsedTrack(track);

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
     );
};
