open Js.Promise;

let device = Config.device;

let getCurrentTrack = () =>
  device->Sonos.Methods.Track.current()
  |> then_(current => Sonos.Decode.CurrentTrack.make(current)->resolve)
  |> catch(Utils.handleError("getCurrentTrack"));

let getPlayingState = () =>
  device->Sonos.Methods.PlayerControl.State.get()
  |> then_(playState =>
       playState |> Sonos.Decode.CurrentPlayState.make |> resolve
     )
  |> catch(Utils.handleError("getPlayingState"));
