open Sonos;

let device = Sonos.device(Devices.Iteam.lounge);

device->setSpotifyRegion(regionEurope);

let queue = (track, sendMessage) => {
  let parsedTrack = track |> Js.String.replaceByRe([%re "/(<|>)/g"], "");

  Js.Promise.(
    device->queue(parsedTrack)
    |> then_(value => {
         let response = value |> SonosDecode.queueResponse;

         sendMessage(
           "Queued track in position " ++ response.firstTrackNumberEnqueued,
         );

         response |> resolve;
       })
  )
  |> ignore;
};

let playTrack = () =>
  Js.Promise.(device->play() |> then_(value => value |> resolve)) |> ignore;

let pause = sendMessage =>
  Js.Promise.(
    device->pause()
    |> then_(value => {
         sendMessage("Paused");
         value |> resolve;
       })
  )
  |> ignore;

let clearPlaylist = sendMessage =>
  Js.Promise.(
    device->Sonos.flush()
    |> then_(value => {
         sendMessage("Cleared queue");
         value |> resolve;
       })
  )
  |> ignore;

let setVolume = (volume: string, sendMessage) =>
  Js.Promise.(
    device->setVolume(volume |> float_of_string)
    |> then_(value => {
         sendMessage("Volume set to " ++ volume);
         value |> resolve;
       })
  )
  |> ignore;

let mute = isMuted =>
  Js.Promise.(device->setMuted(isMuted) |> then_(value => value |> resolve))
  |> ignore;

let previousTrack = () =>
  Js.Promise.(device->previous() |> then_(value => value |> resolve)) |> ignore;

let nextTrack = () =>
  Js.Promise.(device->next() |> then_(value => value |> resolve)) |> ignore;

let currentQueue = sendMessage =>
  Js.Promise.(
    device->getQueue()
    |> then_(value => {
         sendMessage("Tracks in queue");
         value |> resolve;
       })
  )
  |> ignore;

let parseDuration = duration => duration *. 1000. |> Duration.parse;

let nowPlaying = sendMessage =>
  Js.Promise.(
    device->currentTrack()
    |> then_(value => {
         let response = value |> SonosDecode.currentTrackResponse;

         let track =
           response##artist
           ++ " - "
           ++
           response##title
           ++ " ("
           ++
           response##album
           ++ ")";
         let position =
           (response##position |> parseDuration)
           ++ "/"
           ++ (response##duration |> parseDuration);

         sendMessage(
           "*Currently playing*\n"
           ++ track
           ++ "\n Position in queue "
           ++ string_of_int(int_of_float(response##queuePosition))
           ++ " - "
           ++ position,
         );
         value |> resolve;
       })
  )
  |> ignore;

let queueEasterEgg = (track, sendMessage) => {
  sendMessage |> clearPlaylist;

  for (_ in 1 to 10) {
    sendMessage |> queue(track);
  };

  playTrack() |> ignore;
};
