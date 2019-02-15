open Jest;
open Emoji;
open Expect;

test("has a list of christmas songs", () =>
  expect(Christmas.songs) |> toMatchSnapshot
);

test("should get a random christmas song", () =>
  expect(Christmas.randomTrack())
  |> toMatchRe([%re "/^spotify:track:[a-z0-9]+$/ig"])
);
