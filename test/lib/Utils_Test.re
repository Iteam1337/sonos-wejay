open TestFramework;

describe("Utils", ({test, _}) =>
  test("Spotify.Utils.parse_spotify_uri", ({expect}) => {
    let spotify_uri = "spotify:track:5i4zbHovxe43ohUdiso6Od";

    expect.string(spotify_uri |> Wejay.Spotify.Utils.parse_spotify_uri).
      toEqual(
      "5i4zbHovxe43ohUdiso6Od",
    );
  })
);
