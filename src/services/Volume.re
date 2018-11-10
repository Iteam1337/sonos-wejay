open Js.Promise;

let currentVolume = sendMessage =>
  Sonos.device->Sonos.getVolume()
  |> then_(volume => {
       sendMessage("Current volume is " ++ (volume |> Utils.cleanFloat));
       volume |> resolve;
     })
  |> catch(Utils.handleError("currentVolume"))
  |> ignore;

let updateVolume = (volume: string, sendMessage) =>
  Sonos.device->Sonos.setVolume(volume |> float_of_string)
  |> then_(_ => {
       sendMessage("Volume set to " ++ volume);
       volume |> resolve;
     })
  |> catch(Utils.handleError("updateVolume"))
  |> ignore;

let updateVolumeWithValue = (volumeValue, sendMessage) =>
  Sonos.device->Sonos.getVolume()
  |> then_(currentVolume => {
       let newVolume =
         switch (currentVolume, volumeValue) {
         | (0., v) when v < 0. => 0.
         | (c, v) => c +. v
         };

       Sonos.device->Sonos.setVolume(newVolume)
       |> then_(_ => {
            sendMessage("Volume set to " ++ Utils.cleanFloat(newVolume));
            true |> resolve;
          });
     })
  |> catch(Utils.handleError("updateVolumeWithValue"))
  |> ignore;
