open Js.Promise;
open Sonos.Methods;

let currentVolume = sendMessage =>
  Config.device->getVolume()
  |> then_(volume => {
       sendMessage("Current volume is " ++ (volume |> Utils.cleanFloat));
       resolve(volume);
     })
  |> catch(Utils.handleError("currentVolume"))
  |> ignore;

let updateVolume = (volume: string, sendMessage) =>
  Config.device->setVolume(volume |> float_of_string)
  |> then_(_ => {
       sendMessage("Volume set to " ++ volume);
       resolve(volume);
     })
  |> catch(Utils.handleError("updateVolume"))
  |> ignore;

let updateGroupVolume = (volume: string, sendMessage) =>
  groupRenderingControl(Config.wejayIp)##_request(
    "SetGroupVolume",
    groupReqArgs(~instanceId=0, ~volume),
  )
  |> then_(_status => {
       sendMessage("Volume set to " ++ volume);
       resolve(true);
     })
  |> ignore;

let updateVolumeWithValue = (volumeValue, sendMessage) =>
  Config.device->getVolume()
  |> then_(currentVolume => {
       let newVolume =
         switch (currentVolume, volumeValue) {
         | (0., v) when v < 0. => 0.
         | (c, v) => c +. v
         };

       Config.device->setVolume(newVolume)
       |> then_(_ => {
            sendMessage("Volume set to " ++ Utils.cleanFloat(newVolume));
            resolve(true);
          });
     })
  |> catch(Utils.handleError("updateVolumeWithValue"))
  |> ignore;

let controlVolume = (query, sendMessage) =>
  switch (query) {
  | "" => currentVolume(sendMessage)
  | _ => updateGroupVolume(query, sendMessage)
  };

let current = () =>
  Config.device->getVolume()
  |> then_(volume =>
       `Ok("Current volume is " ++ (volume |> Utils.cleanFloat)) |> resolve
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
            `Ok("Volume set to " ++ Utils.cleanFloat(newVolume)) |> resolve
          );
     })
  |> catch(_ => `Failed("Cannot update volume") |> resolve);

let control = volume =>
  switch (volume) {
  | "" => current()
  | _ => update(volume)
  };
