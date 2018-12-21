open Sonos.Methods;
open Js.Promise;

let device = Config.device;

let getCurrentTrack = () =>
  device->currentTrack()
  |> then_(current => Sonos.Decode.currentTrackResponse(current)->resolve)
  |> catch(Utils.handleError("getCurrentTrack"));

let getPlayingState = () =>
  device->getCurrentState()
  |> then_(playState =>
       playState |> Sonos.Decode.currentPlayingState |> resolve
     )
  |> catch(Utils.handleError("getPlayingState"));

let nowPlayingData =
    (
      {artist, title, album, position, duration, queuePosition}: Sonos.Decode.currentTrackResponse,
    ) =>
  switch (queuePosition) {
  | 0. => Messages.nothingIsPlaying
  | _ =>
    let track =
      Utils.artistAndTitle(~artist, ~title)
      ++ " ("
      ++ Belt.Option.getWithDefault(album, "N/A")
      ++ ")";

    let position =
      (position |> Utils.parseDuration)
      ++ "/"
      ++ (duration |> Utils.parseDuration);

    "*Currently playing*\n"
    ++ track
    ++ "\nPosition in queue "
    ++ (queuePosition |> Utils.cleanFloat)
    ++ " - "
    ++ position;
  };

let nowPlaying = sendMessage =>
  getCurrentTrack()
  |> then_(track => {
       track->nowPlayingData->sendMessage;
       true |> resolve;
     })
  |> catch(Utils.handleError("nowPlaying"))
  |> ignore;
