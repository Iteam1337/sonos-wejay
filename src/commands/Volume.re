let current = () => {
  let%Async volume = Config.device->Sonos.Methods.PlayerControl.Volume.get();

  Slack.Msg.make([
    `Section("Current volume is " ++ (volume |> Utils.cleanFloat)),
  ]);
};

let update = volumeValue => {
  let%Async currentVolume =
    Config.device->Sonos.Methods.PlayerControl.Volume.get();
  let newVolume =
    switch (currentVolume, float_of_string(volumeValue)) {
    | (0., v) when v < 0. => 0.
    | (c, v) => c +. v
    };

  let%Async _ =
    Config.device->Sonos.Methods.PlayerControl.Volume.set(newVolume);

  Slack.Msg.make([
    `Section("Volume set to " ++ Utils.cleanFloat(newVolume)),
  ]);
};

let control =
  fun
  | "" => current()
  | volume => update(volume);
