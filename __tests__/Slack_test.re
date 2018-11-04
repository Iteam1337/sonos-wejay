open Jest;
open Slack;

describe("Slack", () =>
  Expect.(
    describe("#userId", () =>
      test("should a parsed user id", () =>
        expect(userId("1337")) |> toEqual("<@1337>")
      )
    )
  )
);
