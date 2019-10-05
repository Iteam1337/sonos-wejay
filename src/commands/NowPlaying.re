let message = (~sonos, ~cover) => {
  let {artist, title, album, position, duration}: Sonos.Decode.CurrentTrack.t = sonos;

  let trackDuration = Utils.parseDuration(duration);
  let currentPosition = Utils.parseDuration(position);

  Slack.Block.make([
    `FieldsWithImage({
      accessory: `Image((cover, "Album cover")),
      fields: [
        `Text({j|*Artist*\n$artist|j}),
        `Text({j|*Track name*\n$title|j}),
        `Text({j|*Album*\n$album|j}),
        `Text({j|*Current position*\n$currentPosition / $trackDuration|j}),
      ],
    }),
  ]);
};

let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_((sonos: Sonos.Decode.CurrentTrack.t) => {
         let uri = Utils.sonosUriToSpotifyUri(sonos.uri);
         let id = SpotifyUtils.trackId(uri);

         switch (id) {
         | None =>
           `Ok(Slack.Block.make([`Section("Nothing is currently playing")]))
           |> resolve
         | Some(id) =>
           Spotify.getSpotifyTrack(id)
           |> then_(spotifyTrack => {
                let {cover}: Spotify.WejayTrack.t = spotifyTrack;

                `Ok(message(~cover, ~sonos)) |> resolve;
              })
         };
       })
    |> catch(_ => `Failed("Now playing failed") |> resolve)
  );
