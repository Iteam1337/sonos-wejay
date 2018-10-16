open Jest;
open Duration;

describe("Duration", () =>
  Expect.(
    describe("#parse", () => {
      test("returns time with h:min:s", () =>
        expect(parse(5000000.)) |> toEqual("1:23:19")
      );

      test("returns time with min:s", () =>
        expect(parse(500000.)) |> toEqual("8:20")
      );

      test("returns time with 0:s", () =>
        expect(parse(50000.)) |> toEqual("0:50")
      );

      test("returns when seconds are super close to 60", () =>
        expect(parse(59999.)) |> toEqual("0:59")
      );

      test("returns when seconds are exactly 60", () =>
        expect(parse(60000.)) |> toEqual("1:00")
      );

      test("returns when minutes are super close to 60", () =>
        expect(parse(3599999.)) |> toEqual("59:59")
      );

      test("returns when minutes are exactly 60", () =>
        expect(parse(3600000.)) |> toEqual("1:00:00")
      );
    })
  )
);
