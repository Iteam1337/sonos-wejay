open Sonos;
open Js.Promise;

let asLast = (track, user, sendMessage) => {
  let parsedTrack = Utils.parsedTrack(track);

  switch (user) {
  | Some(u) => Database.insertTrack(~uri=parsedTrack, ~user=u)
  | None => ()
  };

  let%Await queuedTrack = device->queueAsLast(parsedTrack);
  let%Await {queuePosition} = Services.getCurrentTrack();

  let {firstTrackNumberEnqueued} = queuedTrack |> SonosDecode.queueResponse;

  let queuedPosition =
    int_of_string(firstTrackNumberEnqueued)
    - int_of_float(queuePosition)
    |> string_of_int;

  sendMessage(
    "Sweet! Your track is number *"
    ++ queuedPosition
    ++ "* in the queue :musical_note:",
  );

  true |> resolve;
};

let asNext = (track, user, sendMessage) => {
  let parsedTrack = Utils.parsedTrack(track);

  switch (user) {
  | Some(u) => Database.insertTrack(~uri=parsedTrack, ~user=u)
  | None => ()
  };

  let%Await {queuePosition} = Services.getCurrentTrack();
  let%Await _ = device->queue(parsedTrack, int_of_float(queuePosition) + 1);

  sendMessage("Your track will play right after the current");
};

let listTracks = (tracks: array(Sonos.currentQueue)) =>
  tracks
  ->Belt.Array.mapWithIndex((i, {artist, title}) =>
      Utils.listNumber(i) ++ Utils.artistAndTitle(~artist, ~title)
    );

let currentQueue = sendMessage => {
  let%Await queue = device->getQueue();
  let%Await {queuePosition} = Services.getCurrentTrack();

  let {items} = queue |> SonosDecode.currentQueueResponse;

  let tracks =
    items
    ->Belt.Array.slice(
        ~offset=queuePosition |> int_of_float,
        ~len=Belt.Array.length(items),
      )
    ->listTracks
    |> Js.Array.joinWith("\n");

  sendMessage("*Upcoming tracks*\n" ++ tracks);
};

let getFullQueue = sendMessage => {
  let%Await queue = device->getQueue();

  let {items} = queue |> SonosDecode.currentQueueResponse;
  let tracks = items->listTracks |> Js.Array.joinWith("\n");
  sendMessage(tracks);
};

let clearQueue = sendMessage => {
  let%Await _ = device->Sonos.flush();

  sendMessage("Cleared queue");
};
