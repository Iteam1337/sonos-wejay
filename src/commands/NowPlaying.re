let run = () => {
  let%Async {artist, title, album, position, duration, uri} =
    Services.getCurrentTrack();

  switch (uri |> Utils.Sonos.toSpotifyUri |> SpotifyUtils.trackId) {
  | None => Slack.Msg.make([`Section("Nothing is currently playing")])
  | Some(id) =>
    let%Async track = Spotify.Track.make(id);

    let trackDuration = Duration.make(duration);
    let currentPosition = Duration.make(position);

    Slack.Msg.make([
      `FieldsWithImage({
        accessory: `Image((track.cover, "Album cover")),
        fields: [
          `Text({j|*Artist*\n$artist|j}),
          `Text({j|*Track name*\n$title|j}),
          `Text({j|*Album*\n$album|j}),
          `Text({j|*Current position*\n$currentPosition / $trackDuration|j}),
        ],
      }),
    ]);
  };
};
