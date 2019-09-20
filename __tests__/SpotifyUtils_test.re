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

  describe("#trackId", () =>
    test("should return an id for track", () =>
      expect(trackId("spotify:track:3Bj2mrlp3tALHO5U3mK8zM"))
      |> toEqual("3Bj2mrlp3tALHO5U3mK8zM")
    )
  );
});
