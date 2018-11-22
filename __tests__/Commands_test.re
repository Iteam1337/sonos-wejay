open Jest;
open Commands;

describe("Commands", () => {
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

  describe("#decodeCommand", () => {
    describe("SpotifyCopy", () =>
      test("should add spotify copy and paste", () =>
        expect(
          decodeCommand(
            "<https://open.spotify.com/track/2mPE346iPVnss0GoRgwGTK>\n<https://open.spotify.com/track/4LT7xQZgICM6FqUJkWI6aM>",
          ),
        )
        |> toEqual(
             SpotifyCopy([|
               "spotify:track:2mPE346iPVnss0GoRgwGTK",
               "spotify:track:4LT7xQZgICM6FqUJkWI6aM",
             |]),
           )
      )
    );

    describe("#Emoji", () => {
      test(":+1:", () =>
        expect(decodeCommand(":+1:")) |> toEqual(Emoji(ThumbsUp))
      );

      test(":-1:", () =>
        expect(decodeCommand(":-1:")) |> toEqual(Emoji(ThumbsDown))
      );

      test(":santa:", () =>
        expect(decodeCommand(":santa:")) |> toEqual(Emoji(Santa))
      );
    });

    test("clear", () =>
      expect(decodeCommand("clear")) |> toEqual(Clear)
    );

    test("currentqueue", () =>
      expect(decodeCommand("currentqueue")) |> toEqual(CurrentQueue)
    );

    test("getqueue", () =>
      expect(decodeCommand("getqueue")) |> toEqual(CurrentQueue)
    );

    test("freebird", () =>
      expect(decodeCommand("freebird")) |> toEqual(EasterEgg(FreeBird))
    );

    test("friday", () =>
      expect(decodeCommand("friday")) |> toEqual(EasterEgg(Friday))
    );

    test("fullqueue", () =>
      expect(decodeCommand("fullqueue")) |> toEqual(FullQueue)
    );

    test("classics", () =>
      expect(decodeCommand("classics"))
      |> toEqual(EasterEgg(IteamClassics))
    );

    test("shoreline", () =>
      expect(decodeCommand("shoreline")) |> toEqual(EasterEgg(Shoreline))
    );

    test("slowdance", () =>
      expect(decodeCommand("slowdance")) |> toEqual(EasterEgg(Slowdance))
    );

    test("tequila", () =>
      expect(decodeCommand("tequila")) |> toEqual(EasterEgg(Tequila))
    );

    test("help", () =>
      expect(decodeCommand("help")) |> toEqual(Help)
    );

    test("l (library short-hand)", () =>
      expect(decodeCommand("l")) |> toEqual(Library)
    );

    test("library", () =>
      expect(decodeCommand("library")) |> toEqual(Library)
    );

    test("mute", () =>
      expect(decodeCommand("mute")) |> toEqual(Mute)
    );

    test("next", () =>
      expect(decodeCommand("next")) |> toEqual(Next)
    );

    test("np (nowplaying short-hand)", () =>
      expect(decodeCommand("np")) |> toEqual(NowPlaying)
    );

    test("nowplaying", () =>
      expect(decodeCommand("nowplaying")) |> toEqual(NowPlaying)
    );

    test("pause", () =>
      expect(decodeCommand("pause")) |> toEqual(Pause)
    );

    test("play", () =>
      expect(decodeCommand("play")) |> toEqual(Play)
    );

    test("playtrack", () =>
      expect(decodeCommand("playtrack")) |> toEqual(PlayTrack)
    );

    test("previous", () =>
      expect(decodeCommand("previous")) |> toEqual(Previous)
    );

    test("q (queue short-hand)", () =>
      expect(decodeCommand("q")) |> toEqual(Queue)
    );

    test("queue", () =>
      expect(decodeCommand("queue")) |> toEqual(Queue)
    );

    test("s (search short-hand)", () =>
      expect(decodeCommand("s")) |> toEqual(Search)
    );

    test("search", () =>
      expect(decodeCommand("search")) |> toEqual(Search)
    );

    test("unmute", () =>
      expect(decodeCommand("unmute")) |> toEqual(Unmute)
    );

    test("unknowncommand", () =>
      expect(decodeCommand("unknowncommand"))
      |> toEqual(UnknownCommand("unknowncommand"))
    );

    test("volume", () =>
      expect(decodeCommand("volume")) |> toEqual(Volume)
    );
  });
});
