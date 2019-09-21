let message = (~sonos, ~cover) => {
  let {artist, title, album, position, duration}: Sonos.Decode.currentTrackResponse = sonos;

  let trackDuration = Utils.parseDuration(duration);
  let currentPosition = Utils.parseDuration(position);

  Slack.Block.(
    [|
      Fields.make(
        ~fields=[|
          Text.make(~text={j|*Artist*\n$artist|j}, ()),
          Text.make(~text={j|*Track name*\n$title|j}, ()),
          Text.make(~text={j|*Album*\n$album|j}, ()),
          Text.make(
            ~text={j|*Current position*\n$currentPosition / $trackDuration|j},
            (),
          ),
        |],
        ~accessory=Image.make(~image_url=cover, ~alt_text="Album cover", ()),
        (),
      ),
    |]
  );
};

let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_((sonos: Sonos.Decode.currentTrackResponse) => {
         let uri = Utils.sonosUriToSpotifyUri(sonos.uri);
         let id = SpotifyUtils.trackId(uri);

         Spotify.getSpotifyTrack(id)
         |> then_(spotifyTrack => {
              let {cover}: Spotify.WejayTrack.t = spotifyTrack;

              `Ok(("", message(~cover, ~sonos))) |> resolve;
            });
       })
    |> catch(_ => `Failed("Now playing failed") |> resolve)
  );
