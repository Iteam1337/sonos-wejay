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
