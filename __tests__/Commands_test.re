open Jest;
open Commands;
open Expect;

describe("parseCommand", () => {
  test("removes user", () =>
    expect(parseCommand("<@UD8UR2GGP> s doors end")) |> toEqual("s")
  );

  test("lower cases text", () =>
    expect(parseCommand("Search WOOP")) |> toEqual("search")
  );

  test("lower cases text", () =>
    expect(parseCommand("<@UD8UR2GGP> volume")) |> toEqual("volume")
  );
});

describe("#make", () => {
  describe("SpotifyCopy", () => {
    test("should add spotify copy and paste", () =>
      expect(
        make(
          Some(
            "<https://open.spotify.com/track/2mPE346iPVnss0GoRgwGTK>\n<https://open.spotify.com/track/4LT7xQZgICM6FqUJkWI6aM>",
          ),
        ),
      )
      |> toEqual(
           SpotifyCopy([|
             "spotify:track:2mPE346iPVnss0GoRgwGTK",
             "spotify:track:4LT7xQZgICM6FqUJkWI6aM",
           |]),
         )
    );

    test("should add spotify playlists from copied link", () =>
      expect(
        make(
          Some(
            "https://open.spotify.com/user/believer/playlist/77ffhasgb5saaregvpnxwd?si=e-l_i0jdt7oq370tn23ccg",
          ),
        ),
      )
      |> toEqual(
           SpotifyCopy([|
             "spotify:user:believer:playlist:77ffhasgb5saaregvpnxwd",
           |]),
         )
    );
  });

  describe("#Emoji", () => {
    test(":thumbsup:", () =>
      expect(make(Some(":thumbsup:"))) |> toEqual(Emoji(ThumbsUp))
    );

    test(":thumbsdown:", () =>
      expect(make(Some(":thumbsdown:"))) |> toEqual(Emoji(ThumbsDown))
    );

    test(":+1:", () =>
      expect(make(Some(":+1:"))) |> toEqual(Emoji(ThumbsUp))
    );

    test(":-1:", () =>
      expect(make(Some(":-1:"))) |> toEqual(Emoji(ThumbsDown))
    );

    test(":santa:", () =>
      expect(make(Some(":santa:"))) |> toEqual(Emoji(Santa))
    );
  });

  test("clear", () =>
    expect(make(Some("clear"))) |> toEqual(Clear)
  );

  test("cq", () =>
    expect(make(Some("cq"))) |> toEqual(CurrentQueue)
  );

  test("currentqueue", () =>
    expect(make(Some("currentqueue"))) |> toEqual(CurrentQueue)
  );

  test("gq", () =>
    expect(make(Some("gq"))) |> toEqual(CurrentQueue)
  );

  test("getqueue", () =>
    expect(make(Some("getqueue"))) |> toEqual(CurrentQueue)
  );

  test("freebird", () =>
    expect(make(Some("freebird"))) |> toEqual(EasterEgg(FreeBird))
  );

  test("friday", () =>
    expect(make(Some("friday"))) |> toEqual(EasterEgg(Friday))
  );

  test("fq", () =>
    expect(make(Some("fq"))) |> toEqual(FullQueue)
  );

  test("fullqueue", () =>
    expect(make(Some("fullqueue"))) |> toEqual(FullQueue)
  );

  test("classics", () =>
    expect(make(Some("classics"))) |> toEqual(EasterEgg(IteamClassics))
  );

  test("shoreline", () =>
    expect(make(Some("shoreline"))) |> toEqual(EasterEgg(Shoreline))
  );

  test("slowdance", () =>
    expect(make(Some("slowdance"))) |> toEqual(EasterEgg(Slowdance))
  );

  test("tequila", () =>
    expect(make(Some("tequila"))) |> toEqual(EasterEgg(Tequila))
  );

  test("help", () =>
    expect(make(Some("help"))) |> toEqual(Help)
  );

  test("mute", () =>
    expect(make(Some("mute"))) |> toEqual(Mute)
  );

  test("next", () =>
    expect(make(Some("next"))) |> toEqual(Next)
  );

  test("np (nowplaying short-hand)", () =>
    expect(make(Some("np"))) |> toEqual(NowPlaying)
  );

  test("nowplaying", () =>
    expect(make(Some("nowplaying"))) |> toEqual(NowPlaying)
  );

  test("pause", () =>
    expect(make(Some("pause"))) |> toEqual(Pause)
  );

  test("play", () =>
    expect(make(Some("play"))) |> toEqual(Play)
  );

  test("playtrack", () =>
    expect(make(Some("playtrack"))) |> toEqual(PlayTrack)
  );

  test("previous", () =>
    expect(make(Some("previous"))) |> toEqual(Previous)
  );

  test("q (queue short-hand)", () =>
    expect(make(Some("q"))) |> toEqual(Queue)
  );

  test("queue", () =>
    expect(make(Some("queue"))) |> toEqual(Queue)
  );

  test("s (search short-hand)", () =>
    expect(make(Some("s"))) |> toEqual(Search)
  );

  test("search", () =>
    expect(make(Some("search"))) |> toEqual(Search)
  );

  test("unmute", () =>
    expect(make(Some("unmute"))) |> toEqual(Unmute)
  );

  test("unknowncommand", () =>
    expect(make(Some("unknowncommand")))
    |> toEqual(UnknownCommand("unknowncommand"))
  );

  test("volume", () =>
    expect(make(Some("volume"))) |> toEqual(Volume)
  );

  test("www", () =>
    expect(make(Some("www"))) |> toEqual(EasterEgg(WWW))
  );

  test("worldwideweb", () =>
    expect(make(Some("worldwideweb"))) |> toEqual(EasterEgg(WWW))
  );
});
