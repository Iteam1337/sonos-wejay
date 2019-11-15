open Jest;
open Utils;
open Expect;

describe("#artistAndTitle", () =>
  test("artist and title combined", () =>
    expect(
      artistAndTitle(
        ~artist="Bring Me The Horizon",
        ~title="Hospital for Souls",
      ),
    )
    |> toEqual("Bring Me The Horizon - Hospital for Souls")
  )
);

describe("#listNumber", () =>
  test("create a list number item", () =>
    expect(listNumber(0)) |> toEqual("1. ")
  )
);

describe("#parsedTrack", () => {
  test("parses Spotify URI", () =>
    expect(parsedTrack("spotify:track:4UQLQJu3DNvVkMVglwElU2"))
    |> toEqual("spotify:track:4UQLQJu3DNvVkMVglwElU2")
  );

  test("parses Spotify URI with Slack brackets", () =>
    expect(parsedTrack("<spotify:track:4UQLQJu3DNvVkMVglwElU2>"))
    |> toEqual("spotify:track:4UQLQJu3DNvVkMVglwElU2")
  );

  test("parses spotify url", () =>
    expect(
      parsedTrack(
        "https://open.spotify.com/track/6dudjphptpe3zmta9jdcex?si=pufhmxrgtpc89vbp88caow",
      ),
    )
    |> toEqual("spotify:track:6dudjphptpe3zmta9jdcex")
  );

  test("parses spotify url", () =>
    expect(
      parsedTrack("https://open.spotify.com/track/5fxDE80ZPA32JXjZN87pQJ"),
    )
    |> toEqual("spotify:track:5fxDE80ZPA32JXjZN87pQJ")
  );
});

describe("#cleanFloat", () =>
  test("cleans up a float to string", () =>
    expect(cleanFloat(50.)) |> toEqual("50")
  )
);

describe("#parseSpotifyCopy", () => {
  test("should parse spotify copy/paste", () =>
    expect(
      parseSpotifyCopy(
        "<https://open.spotify.com/track/2mPE346iPVnss0GoRgwGTK>\n<https://open.spotify.com/track/4LT7xQZgICM6FqUJkWI6aM>",
      ),
    )
    |> toEqual([|
         "spotify:track:2mPE346iPVnss0GoRgwGTK",
         "spotify:track:4LT7xQZgICM6FqUJkWI6aM",
       |])
  );

  test("should parse spotify playlist links", () =>
    expect(
      parseSpotifyCopy(
        "https://open.spotify.com/playlist/77ffhasgb5saaregvpnxwd?si=e-l_i0jdt7oq370tn23ccg",
      ),
    )
    |> toEqual([|"spotify:playlist:77ffhasgb5saaregvpnxwd"|])
  );
});

describe("RandomTrack", () =>
  test("it returns a random mock search", () =>
    expect(RandomTrack.make(["test", "test2"]) |> Js.String.length)
    |> toBeGreaterThan(0)
  )
);

describe("Parse", () => {
  describe("Track", () => {
    test("handles URI", () =>
      expect(Parse.make("spotify:track:4UQLQJu3DNvVkMVglwElU2"))
      |> toEqual(Parse.Track("spotify:track:4UQLQJu3DNvVkMVglwElU2"))
    );

    test("handles URI with Slack brackets", () =>
      expect(Parse.make("<spotify:track:4UQLQJu3DNvVkMVglwElU2>"))
      |> toEqual(Parse.Track("spotify:track:4UQLQJu3DNvVkMVglwElU2"))
    );

    test("handles URL", () =>
      expect(
        Parse.make(
          "https://open.spotify.com/track/6dudjphptpe3zmta9jdcex?si=pufhmxrgtpc89vbp88caow",
        ),
      )
      |> toEqual(Parse.Track("spotify:track:6dudjphptpe3zmta9jdcex"))
    );

    test("handles URL without query params", () =>
      expect(
        Parse.make("https://open.spotify.com/track/5fxDE80ZPA32JXjZN87pQJ"),
      )
      |> toEqual(Parse.Track("spotify:track:5fxDE80ZPA32JXjZN87pQJ"))
    );
  });

  describe("Playlist", () => {
    test("handles URL", () =>
      expect(
        Parse.make(
          "https://open.spotify.com/playlist/5DhAWTN2iJ21BivcrMGqUy?si=YxmHGjm-QRyMJH7bQqBYjg",
        ),
      )
      |> toEqual(Parse.Playlist("spotify:playlist:5DhAWTN2iJ21BivcrMGqUy"))
    );

    test("handles URL without query params", () =>
      expect(
        Parse.make(
          "https://open.spotify.com/playlist/5DhAWTN2iJ21BivcrMGqUy",
        ),
      )
      |> toEqual(Parse.Playlist("spotify:playlist:5DhAWTN2iJ21BivcrMGqUy"))
    );

    test("handles URI", () =>
      expect(Parse.make("spotify:playlist:5DhAWTN2iJ21BivcrMGqUy"))
      |> toEqual(Parse.Playlist("spotify:playlist:5DhAWTN2iJ21BivcrMGqUy"))
    );
  });
});
