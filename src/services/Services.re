open Sonos;
open Js.Promise;

device->setSpotifyRegion(regionEurope);

let getPlayingState = () =>
  device->Sonos.getCurrentState()
  |> then_(playState =>
       playState |> SonosDecode.currentPlayingState |> resolve
     );

let getCurrentTrack = () => {
  let%Await current = device->currentTrack();

  SonosDecode.currentTrackResponse(current);
};

let nowPlaying = sendMessage => {
  let%Await {artist, title, album, position, duration, queuePosition} =
    getCurrentTrack();
  let track =
    Utils.artistAndTitle(~artist, ~title)
    ++ " ("
    ++ Belt.Option.getWithDefault(album, "N/A")
    ++ ")";

  let position =
    (position |> Utils.parseDuration)
    ++ "/"
    ++ (duration |> Utils.parseDuration);

  sendMessage(
    "*Currently playing*\n"
    ++ track
    ++ "\n Position in queue "
    ++ (queuePosition |> Utils.cleanFloat)
    ++ " - "
    ++ position,
  );
};
