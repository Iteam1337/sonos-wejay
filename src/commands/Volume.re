open Js.Promise;

let current = () =>
  Config.device->Sonos.Methods.PlayerControl.Volume.get()
  |> then_(volume =>
       `Ok(
         Slack.Block.make([
           `Section("Current volume is " ++ (volume |> Utils.cleanFloat)),
         ]),
       )
       |> resolve
     )
  |> catch(_ => `Failed("Cannot get current volume") |> resolve);

let update = volumeValue =>
  Config.device->Sonos.Methods.PlayerControl.Volume.get()
  |> then_(currentVolume => {
       let newVolume =
         switch (currentVolume, float_of_string(volumeValue)) {
         | (0., v) when v < 0. => 0.
         | (c, v) => c +. v
         };

       Config.device->Sonos.Methods.PlayerControl.Volume.set(newVolume)
       |> then_(_ =>
            `Ok(
              Slack.Block.make([
                `Section("Volume set to " ++ Utils.cleanFloat(newVolume)),
              ]),
            )
            |> resolve
          );
     })
  |> catch(_ => `Failed("Cannot update volume") |> resolve);

let control =
  fun
  | "" => current()
  | volume => update(volume);
