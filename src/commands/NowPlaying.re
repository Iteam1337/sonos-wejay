let nowPlayingData =
    (
      {artist, title, album, position, duration, queuePosition}: Sonos.Decode.currentTrackResponse,
    ) =>
  switch (queuePosition) {
  | 0. => Messages.nothingIsPlaying
  | _ =>
    let track =
      Utils.artistAndTitle(~artist, ~title)
      ++ " ("
      ++ Belt.Option.getWithDefault(album, "N/A")
      ++ ")";

    let position =
      (position |> Utils.parseDuration)
      ++ "/"
      ++ (duration |> Utils.parseDuration);

    "*Currently playing*\n"
    ++ track
    ++ "\nPosition in queue "
    ++ (queuePosition |> Utils.cleanFloat)
    ++ " - "
    ++ position;
  };

let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(track => `Ok(track |> nowPlayingData) |> resolve)
    |> catch(_ => `Failed("Now playing failed") |> resolve)
  );
