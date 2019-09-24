open Jest;
open Emoji;
open Expect;

test("has a list of christmas songs", () =>
  expect(Christmas.songs) |> toMatchSnapshot
);

test("should get a random christmas song", () =>
  expect(Christmas.make())
  |> toMatchRe([%re "/^spotify:track:[a-z0-9]+$/ig"])
);

describe("#emojiCommand", () => {
  test("handles default emojis", () =>
    expect(emojiCommand(":+1:")) |> toEqual(ThumbsUp)
  );

  test("handles skin tone emojis", () =>
    expect(emojiCommand(":+1::skin-tone-1:")) |> toEqual(ThumbsUp)
  );
});

describe("#isEmoji", () => {
  test("handles default emojis", () =>
    expect(isEmoji(":+1:")) |> toBe(true)
  );

  test("handles skin tone emojis", () =>
    expect(isEmoji(":+1::skin-tone-2:")) |> toBe(true)
  );

  test("handles non-emoji commands", () =>
    expect(isEmoji("queue")) |> toBe(false)
  );
});
