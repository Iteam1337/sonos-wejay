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

let nowPlaying = sendMessage =>
  getCurrentTrack()
  |> then_(({artist, title, album, position, duration, queuePosition}) => {
       let track =
         Utils.artistAndTitle(~artist, ~title)
         ++ " ("
         ++ Belt.Option.getWithDefault(album, "N/A")
         ++ ")";

       let position =
         (position |> Utils.parseDuration)
         ++ "/"
         ++ (duration |> Utils.parseDuration);

       sendMessage(
         "*Currently playing*\n"
         ++ track
         ++ "\n Position in queue "
         ++ (queuePosition |> Utils.cleanFloat)
         ++ " - "
         ++ position,
       );

       true |> resolve;
     })
  |> catch(Utils.handleError("nowPlaying"))
  |> ignore;
