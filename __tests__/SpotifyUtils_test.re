open Jest;
open SpotifyUtils;

describe("SpotifyUtils", () => {
  open Expect;

  describe("#toPlaylistUri", () =>
    test("should return a toPlaylistUri uri", () =>
      expect(toPlaylistUri(~user="believer", ~id="1337"))
      |> toEqual("spotify:user:believer:playlist:1337")
    )
  );

  describe("#toUri", () =>
    test("should return a toUri uri", () =>
      expect(toUri("1337")) |> toEqual("spotify:track:1337")
    )
  );

  describe("#spotifySearchUrl", () =>
    test("creates a search url that handles special characters", () =>
      expect(spotifySearchUrl(~query="angels &amp; äirwaves", ()))
      |> toEqual(
           "https://api.spotify.com/v1/search?q=angels%20%26%20%C3%83%C2%A4irwaves&type=track&limit=5&market=SE",
         )
    )
  );
});
