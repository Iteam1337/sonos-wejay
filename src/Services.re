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

let searchLibrary = (q, sendMessageWithAttachments) =>
  Js.Promise.(
    device->searchMusicLibrary("tracks", q)
    |> then_(library => {
         let {items} = library |> SonosDecode.currentQueueResponse;

         items
         |> Array.map((item: Sonos.currentQueue) => {
              let text =
                "*"
                ++ item.artist
                ++ " - "
                ++ item.title
                ++ "*"
                ++ (
                  switch (item.album) {
                  | Some(album) => "\n" ++ album
                  | None => ""
                  }
                );

              Utils.createAttachment(~text, ~uri=item.uri, ());
            })
         |> sendMessageWithAttachments(
              "Searching in music library *" ++ q ++ "*",
            );

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

let queueAsLast = (track, sendMessage) =>
  Js.Promise.(
    device->queueAsLast(Utils.parsedTrack(track))
    |> then_(queuedTrack =>
         getCurrentTrack()
         |> then_(current => {
              let response = queuedTrack |> SonosDecode.queueResponse;

              let queuedPosition =
                int_of_string(response.firstTrackNumberEnqueued)
                - int_of_float(current.queuePosition)
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

let queueAsNext = (track, sendMessage) =>
  Js.Promise.(
    getCurrentTrack()
    |> then_(current =>
         device->queue(
           Utils.parsedTrack(track),
           int_of_float(current.queuePosition) + 1,
         )
         |> then_(() =>
              sendMessage("Your track will play right after the current")
            )
       )
  )
  |> ignore;

let currentQueue = sendMessage =>
  Js.Promise.(
    device->getQueue()
    |> then_(queue =>
         getCurrentTrack()
         |> then_(current => {
              let {items} = queue |> SonosDecode.currentQueueResponse;

              let tracks =
                items
                |> Js.Array.sliceFrom(current.queuePosition |> int_of_float)
                |> Js.Array.mapi((item: Sonos.currentQueue, i) =>
                     string_of_int(i + 1)
                     ++ ". "
                     ++ item.artist
                     ++ " - "
                     ++ item.title
                   )
                |> Js.Array.joinWith("\n");

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
           response.artist
           ++ " - "
           ++ response.title
           ++ " ("
           ++ (
             switch (response.album) {
             | Some(album) => album
             | None => ""
             }
           )
           ++ ")";
         let position =
           (response.position |> Utils.parseDuration)
           ++ "/"
           ++ (response.duration |> Utils.parseDuration);

         sendMessage(
           "*Currently playing*\n"
           ++ track
           ++ "\n Position in queue "
           ++ (response.queuePosition |> Utils.cleanFloat)
           ++ " - "
           ++ position,
         );

         response |> resolve;
       })
    |> catch(Utils.handleError("nowPlaying"))
  )
  |> ignore;

let queueEasterEgg = (track, sendMessage) =>
  sendMessage |> queueAsNext(track);

let getCurrentVolume = sendMessage =>
  Js.Promise.(
    device->getVolume()
    |> then_(volume => {
         sendMessage("Current volume is " ++ (volume |> Utils.cleanFloat));
         volume |> resolve;
       })
    |> catch(Utils.handleError("getVolume"))
  )
  |> ignore;

let setNewVolume = (volume: string, sendMessage) =>
  Js.Promise.(
    device->setVolume(volume |> float_of_string)
    |> then_(value => {
         sendMessage("Volume set to " ++ volume);
         value |> resolve;
       })
    |> catch(Utils.handleError("setVolume"))
  )
  |> ignore;

let blame = sendMessage =>
  Js.Promise.(
    getCurrentTrack()
    |> then_(response => {
         let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

         let uri =
           switch (response.uri |> Js.String.match(spotifyUri)) {
           | Some(match) => Js.Global.decodeURIComponent(match[0])
           | None => ""
           };

         sendMessage |> Database.lastPlay(uri);
         response |> resolve;
       })
  )
  |> ignore;

let playTrackNumber = (trackNumber, sendMessage) =>
  Js.Promise.(
    device->selectTrack(trackNumber |> int_of_string)
    |> then_(_ =>
         getCurrentTrack()
         |> then_(response => {
              sendMessage(
                "*Playing track*\n"
                ++ response.artist
                ++ " - "
                ++ response.title,
              )
              |> ignore;

              resolve(true);
            })
       )
    |> catch(_ => {
         sendMessage(
           "*Cannot play track " ++ trackNumber ++ ". Here's the whole queue*",
         )
         |> ignore;

         device->getQueue()
         |> then_(queue => {
              let {items} = queue |> SonosDecode.currentQueueResponse;

              let tracks =
                items
                |> Js.Array.mapi((item: Sonos.currentQueue, i) =>
                     string_of_int(i + 1)
                     ++ ". "
                     ++ item.artist
                     ++ " - "
                     ++ item.title
                   )
                |> Js.Array.joinWith("\n");

              sendMessage(tracks);
            })
         |> ignore;

         resolve(false);
       })
    |> ignore
  );

let changeVolumeWithValue = (volumeValue, sendMessage) =>
  Js.Promise.(
    device->getVolume()
    |> then_(currentVolume => {
         let newVolume =
           switch (currentVolume, volumeValue) {
           | (0., v) when v < 0. => 0.
           | (c, v) => c +. v
           };

         device->setVolume(newVolume)
         |> then_(_ => {
              sendMessage("Volume set to " ++ Utils.cleanFloat(newVolume));
              true |> resolve;
            });
       })
  )
  |> ignore;
