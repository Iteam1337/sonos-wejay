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
});
