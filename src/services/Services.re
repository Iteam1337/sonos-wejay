open Sonos;
open Js.Promise;

device->setSpotifyRegion(regionEurope);

let getCurrentTrack = () =>
  device->currentTrack()
  |> then_(current => SonosDecode.currentTrackResponse(current)->resolve)
  |> catch(Utils.handleError("getCurrentTrack"));

let getPlayingState = () =>
  device->getCurrentState()
  |> then_(playState =>
       playState |> SonosDecode.currentPlayingState |> resolve
     )
  |> catch(Utils.handleError("getPlayingState"));

let nowPlayingData =
    ({artist, title, album, position, duration, queuePosition}) =>
  switch (queuePosition) {
  | 0. => "Nothing is currently playing, add a track using `search <your track>`"
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
