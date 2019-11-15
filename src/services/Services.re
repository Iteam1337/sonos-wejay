open Js.Promise;

let device = Config.device;

let getCurrentTrack = () =>
  device->Sonos.Methods.Track.current()
  |> then_(track => Sonos.Decode.CurrentTrack.make(track) |> resolve)
  |> catch(err => {
       Js.log(err);
       Js.Promise.reject(
         Js.Exn.raiseError("Something went wrong in getCurrentTrack"),
       );
     });

let getPlayingState = () =>
  device->Sonos.Methods.PlayerControl.State.get()
  |> then_(state => state |> Sonos.Decode.CurrentPlayState.make |> resolve)
  |> catch(err => {
       Js.log(err);
       Js.Promise.reject(
         Js.Exn.raiseError("Something went wrong in getPlayingState"),
       );
     });
