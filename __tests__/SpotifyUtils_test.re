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

    describe("#Christmas", () => {
      test("songs", () =>
        expect(Tracks.Christmas.songs) |> toMatchSnapshot
      );

      test("getSong should return spotify uri", () =>
        expect(Tracks.Christmas.getSong())
        |> toContainString("spotify:track:")
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
});