let message =
    (
      {artist, title, album, position, duration, queuePosition}: Sonos.Decode.currentTrackResponse,
    ) =>
  switch (queuePosition) {
  | 0. => Messages.nothingIsPlaying
  | _ =>
    let q = queuePosition |> Utils.cleanFloat;
    let track = Utils.artistAndTitle(~artist, ~title);
    let album = Belt.Option.getWithDefault(album, "N/A");
    let p = position |> Utils.parseDuration;
    let d = duration |> Utils.parseDuration;

    {j|*Currently playing*\n$track ($album)\nPosition in queue $q - $p/$d|j};
  };

let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(track => `Ok(track |> message) |> resolve)
    |> catch(_ => `Failed("Now playing failed") |> resolve)
  );
