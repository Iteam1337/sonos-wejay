open Sonos.Methods;
open Sonos.Decode;
open Js.Promise;

let device = Config.device;

let trackPosition = (~first, ~queueAt, ()) =>
  int_of_string(first) - int_of_float(queueAt) + 1 |> string_of_int;

let asLast = (track, user, sendMessage) => {
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

            sendMessage(
              "Sweet! Your track is number *"
              ++ trackPosition(
                   ~first=firstTrackNumberEnqueued,
                   ~queueAt=queuePosition,
                   (),
                 )
              ++ "* in the queue :musical_note:",
            );

            true |> resolve;
          })
     )
  |> catch(Utils.handleError("queueAsLast"))
  |> ignore;
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
       |> then_(() =>
            switch (position, queuePosition) {
            | (0., 0.) => sendMessage("Your track will play right now")
            | _ => sendMessage("Your track will play right after the current")
            }
          )
       |> catch(Utils.handleError("queueAsNext"))
     )
  |> ignore;
};

let listTracks = (tracks: array(currentQueue)) =>
  tracks->Belt.Array.mapWithIndex((i, {artist, title}) =>
    Utils.listNumber(i) ++ Utils.artistAndTitle(~artist, ~title)
  );

let currentQueue = sendMessage =>
  device->getQueue()
  |> then_(queue =>
       Services.getCurrentTrack()
       |> then_(({queuePosition}) => {
            let {items} = queue->currentQueueResponse;

            let tracks =
              items
              ->Belt.Array.slice(
                  ~offset=queuePosition |> int_of_float,
                  ~len=Belt.Array.length(items),
                )
              ->listTracks
              |> Js.Array.joinWith("\n");

            sendMessage("*Upcoming tracks*\n" ++ tracks);
            queue |> resolve;
          })
       |> catch(Utils.handleError("currentQueue -> currentTrack"))
     )
  |> catch(Utils.handleError("currentQueue"))
  |> ignore;

let getFullQueue = sendMessage =>
  device->getQueue()
  |> then_(queue => {
       let {items} = queue->currentQueueResponse;
       let tracks = items->listTracks |> Js.Array.joinWith("\n");
       sendMessage(tracks);
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
