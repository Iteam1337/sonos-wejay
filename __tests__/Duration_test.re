open Jest;
open Duration;

describe("Duration", () =>
  Expect.(
    describe("#make", () => {
      test("returns time with h:min:s", () =>
        expect(make(5000000.)) |> toEqual("1:23:19")
      );

      test("returns time with min:s", () =>
        expect(make(500000.)) |> toEqual("8:20")
      );

      test("returns time with 0:s", () =>
        expect(make(50000.)) |> toEqual("0:50")
      );

      test("returns when seconds are super close to 60", () =>
        expect(make(59999.)) |> toEqual("0:59")
      );

      test("returns when seconds are exactly 60", () =>
        expect(make(60000.)) |> toEqual("1:00")
      );

      test("returns when minutes are super close to 60", () =>
        expect(make(3599999.)) |> toEqual("59:59")
      );

      test("returns when minutes are exactly 60", () =>
        expect(make(3600000.)) |> toEqual("1:00:00")
      );

      test("handles seconds", () =>
        expect(make(5000.)) |> toEqual("1:23:19")
      );
    })
  )
);
