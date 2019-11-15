let message = (~sonos, ~cover) => {
  let {artist, title, album, position, duration}: Sonos.Decode.CurrentTrack.t = sonos;

  let trackDuration = Duration.make(duration);
  let currentPosition = Duration.make(position);

  Slack.Msg.make([
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
         let uri = Utils.Sonos.toSpotifyUri(sonos.uri);
         let id = SpotifyUtils.trackId(uri);

         switch (id) {
         | None => Slack.Msg.make([`Section("Nothing is currently playing")])
         | Some(id) =>
           Spotify.Track.make(id)
           |> then_(spotifyTrack => {
                let {cover}: Spotify.Track.t = spotifyTrack;

                message(~cover, ~sonos);
              })
         };
       })
    |> catch(_ => Belt.Result.Error("Now playing failed") |> resolve)
  );
