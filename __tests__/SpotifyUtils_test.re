open Jest;
open SpotifyUtils;

describe("SpotifyUtils", () => {
  open Expect;

  describe("#playlist", () =>
    test("should return a playlist uri", () =>
      expect(playlist(~user="believer", ~id="1337"))
      |> toEqual("spotify:user:believer:playlist:1337")
    )
  );

  describe("#track", () =>
    test("should return a track uri", () =>
      expect(track("1337")) |> toEqual("spotify:track:1337")
    )
  );

  describe("Tracks", () => {
    test("Lynyrd Skynyrd - Free bird", () =>
      expect(Tracks.freeBird)
      |> toEqual("spotify:track:4qsAYBCJnu2OkTKUVbbOF1")
    );

    test("Rebecca Black - Friday", () =>
      expect(Tracks.friday)
      |> toEqual("spotify:track:4fK6E2UywZTJIa5kWnCD6x")
    );

    test("Broder Daniel - Shoreline", () =>
      expect(Tracks.shoreline)
      |> toEqual("spotify:track:77jVczOFXfbdugN4djsIqs")
    );

    test("The Champs - Tequila", () =>
      expect(Tracks.tequila)
      |> toEqual("spotify:track:5gJKsGij5oGt5H5RSFYXPa")
    );

    test("Nick Borgen - World Wide Web", () =>
      expect(Tracks.worldwideweb)
      |> toEqual("spotify:track:597NerobfkV9DhKCySXXWF")
    );

    describe("#Christmas", () => {
      test("songs", () =>
        expect(Tracks.Christmas.songs) |> toMatchSnapshot
      );

      test("getSong should return spotify uri", () =>
        expect(Tracks.Christmas.getSong())
        |> toMatchRe([%re "/^spotify:track:[a-z0-9]+$/ig"])
      );
    });
  });

  describe("Playlists", () => {
    test("Iteam classics", () =>
      expect(Playlists.iteamClassics)
      |> toEqual("spotify:user:believer:playlist:445NQ4LkJFtBsHUOdr3LFI")
    );

    test("Slowdance music", () =>
      expect(Playlists.slowdance)
      |> toEqual("spotify:user:believer:playlist:5DQzhEf0U4Lji5kvXnPYSy")
    );
  });

  describe("#spotifySearchUrl", () =>
    test("creates a search url that handles special characters", () =>
      expect(spotifySearchUrl(~query="angels &amp; äirwaves", ()))
      |> toEqual(
           "https://api.spotify.com/v1/search?q=angels%20%26%20%C3%83%C2%A4irwaves&type=track&limit=5&market=SE",
         )
    )
  );
});
