open Sonos;

let device = Sonos.device(Devices.Iteam.lounge);

device->setSpotifyRegion(regionEurope);

let justResolve = inputFunction =>
  Js.Promise.(
    inputFunction
    |> then_(value => value |> resolve)
    |> catch(Utils.handleError("justResolve"))
  )
  |> ignore;

let playTrack = () => device->play() |> justResolve;
let pause = () => device->pause() |> justResolve;
let mute = isMuted => device->setMuted(isMuted) |> justResolve;
let nextTrack = () => device->next() |> justResolve;
let previousTrack = () => device->previous() |> justResolve;

let getCurrentTrack = () =>
  Js.Promise.(
    device->currentTrack()
    |> then_(current =>
         current |> SonosDecode.currentTrackResponse |> resolve
       )
    |> catch(Utils.handleError("getCurrentTrack"))
  );

let searchLibrary = (q, sendMessage) =>
  Js.Promise.(
    device->searchMusicLibrary("tracks", q)
    |> then_(library => {
         let response = library |> SonosDecode.currentQueueResponse;

         response##items
         |> Array.map(item => {
              let text =
                "*"
                ++ (item |> Utils.trackInfo)
                ++ "*"
                ++ (
                  switch (item##album) {
                  | Some(album) => "\n" ++ album
                  | None => ""
                  }
                );

              Utils.createAttachment(~text, ~uri=item##uri, ());
            })
         |> sendMessage("Searching in music library *" ++ q ++ "*");

         library |> resolve;
       })
    |> catch(Utils.handleError("searchLibrary"))
  )
  |> ignore;

let clearPlaylist = sendMessage =>
  Js.Promise.(
    device->Sonos.flush()
    |> then_(value => {
         sendMessage("Cleared queue");
         value |> resolve;
       })
    |> catch(Utils.handleError("clearPlaylist"))
  )
  |> ignore;

let queue = (track, sendMessage) => {
  let parsedTrack = track |> Js.String.replaceByRe([%re "/(<|>)/g"], "");

  Js.Promise.(
    device->queue(parsedTrack)
    |> then_(queuedTrack =>
         getCurrentTrack()
         |> then_(current => {
              let response = queuedTrack |> SonosDecode.queueResponse;

              let queuedPosition =
                int_of_string(response.firstTrackNumberEnqueued)
                - int_of_float(current##queuePosition)
                |> string_of_int;

              sendMessage(
                "Sweet! Your track is number *"
                ++ queuedPosition
                ++ "* in the queue :musical_note:",
              );

              response |> resolve;
            })
         |> catch(Utils.handleError("queue -> currentTrack"))
       )
    |> catch(Utils.handleError("queue"))
  )
  |> ignore;
};

let currentQueue = sendMessage =>
  Js.Promise.(
    device->getQueue()
    |> then_(queue =>
         getCurrentTrack()
         |> then_(current => {
              let response = queue |> SonosDecode.currentQueueResponse;

              let tracks =
                response##items
                |> Js.Array.sliceFrom(current##queuePosition |> int_of_float)
                |> Js.Array.mapi((item, i) =>
                     string_of_int(i + 1)
                     ++ ". "
                     ++ (item |> Utils.trackInfo)
                     ++ "\n"
                   )
                |> Js.Array.joinWith("");

              sendMessage("*Upcoming tracks*\n" ++ tracks);
              queue |> resolve;
            })
         |> catch(Utils.handleError("currentQueue -> currentTrack"))
       )
    |> catch(Utils.handleError("currentQueue"))
  )
  |> ignore;

let nowPlaying = sendMessage =>
  Js.Promise.(
    getCurrentTrack()
    |> then_(response => {
         let track =
           (response |> Utils.trackInfo)
           ++ " ("
           ++ (
             switch (response##album) {
             | Some(album) => album
             | None => ""
             }
           )
           ++ ")";
         let position =
           (response##position |> Utils.parseDuration)
           ++ "/"
           ++ (response##duration |> Utils.parseDuration);

         sendMessage(
           "*Currently playing*\n"
           ++ track
           ++ "\n Position in queue "
           ++ (response##queuePosition |> Utils.cleanFloat)
           ++ " - "
           ++ position,
         );

         response |> resolve;
       })
    |> catch(Utils.handleError("nowPlaying"))
  )
  |> ignore;

let queueEasterEgg = (track, sendMessage) => {
  sendMessage |> clearPlaylist;

  for (_ in 1 to 10) {
    sendMessage |> queue(track);
  };

  playTrack() |> ignore;
};

let getVolume = sendMessage =>
  Js.Promise.(
    device->getVolume()
    |> then_(volume => {
         sendMessage("Current volume is " ++ (volume |> Utils.cleanFloat));
         volume |> resolve;
       })
    |> catch(Utils.handleError("getVolume"))
  )
  |> ignore;

let setVolume = (volume: string, sendMessage) =>
  Js.Promise.(
    device->setVolume(volume |> float_of_string)
    |> then_(value => {
         sendMessage("Volume set to " ++ volume);
         value |> resolve;
       })
    |> catch(Utils.handleError("setVolume"))
  )
  |> ignore;
