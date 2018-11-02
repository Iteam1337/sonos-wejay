open Jest;
open Decode;

describe("Decode", () =>
  Expect.(
    describe("parseQuery", () => {
      test("removes user", () =>
        expect(parseQuery("<@UD8UR2GGP> s doors end")) |> toEqual("doors end")
      );

      test("handles command query", () =>
        expect(parseQuery("s test")) |> toEqual("test")
      );
    })
  )
);
