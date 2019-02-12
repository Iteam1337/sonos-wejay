open Jest;
open Emoji;
open Expect;

describe("Tracks", () =>
  describe("#Christmas", () => {
    test("songs", () =>
      expect(Christmas.songs) |> toMatchSnapshot
    );

    test("getSong should return spotify uri", () =>
      expect(Christmas.getSong())
      |> toMatchRe([%re "/^spotify:track:[a-z0-9]+$/ig"])
    );
  })
);
