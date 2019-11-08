open Jest;
open Slack;

describe("Slack", () =>
  Expect.(
    describe("#User.make", () =>
      test("should a parsed user id", () =>
        expect(User.make("1337")) |> toEqual("<@1337>")
      )
    )
  )
);
