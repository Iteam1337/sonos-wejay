open Js.Promise;

let currentVolume = sendMessage => {
  let%Await volume = Sonos.device->Sonos.getVolume();

  sendMessage("Current volume is " ++ (volume |> Utils.cleanFloat));
};

let updateVolume = (volume: string, sendMessage) => {
  let%Await _ = Sonos.device->Sonos.setVolume(volume |> float_of_string);

  sendMessage("Volume set to " ++ volume);
};

let updateVolumeWithValue = (volumeValue, sendMessage) => {
  let%Await currentVolume = Sonos.device->Sonos.getVolume();
  let newVolume =
    switch (currentVolume, volumeValue) {
    | (0., v) when v < 0. => 0.
    | (c, v) => c +. v
    };

  let%Await _ = Sonos.device->Sonos.setVolume(newVolume);

  sendMessage("Volume set to " ++ Utils.cleanFloat(newVolume));
};
