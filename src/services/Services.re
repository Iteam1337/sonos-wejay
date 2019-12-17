open Js.Promise;

let device = Config.device;

let getCurrentTrack = () => {
  let%Async track = device->Sonos.Methods.Track.current();

  Sonos.Decode.CurrentTrack.make(track) |> resolve;
};

let getPlayingState = () => {
  let%Async state = device->Sonos.Methods.PlayerControl.State.get();

  Sonos.Decode.CurrentPlayState.make(state) |> resolve;
};
