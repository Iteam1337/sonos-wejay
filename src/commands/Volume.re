open Js.Promise;
open Sonos.Methods;

let current = () =>
  Config.device->getVolume()
  |> then_(volume =>
       `Ok(
         Slack.Block.Simple.make(
           ~message="Current volume is " ++ (volume |> Utils.cleanFloat),
         ),
       )
       |> resolve
     )
  |> catch(_ => `Failed("Cannot get current volume") |> resolve);

let update = volumeValue =>
  Config.device->getVolume()
  |> then_(currentVolume => {
       let newVolume =
         switch (currentVolume, float_of_string(volumeValue)) {
         | (0., v) when v < 0. => 0.
         | (c, v) => c +. v
         };

       Config.device->setVolume(newVolume)
       |> then_(_ =>
            `Ok(
              Slack.Block.Simple.make(
                ~message="Volume set to " ++ Utils.cleanFloat(newVolume),
              ),
            )
            |> resolve
          );
     })
  |> catch(_ => `Failed("Cannot update volume") |> resolve);

let control =
  fun
  | "" => current()
  | volume => update(volume);
