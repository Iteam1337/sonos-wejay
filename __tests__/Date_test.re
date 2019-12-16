open Jest;
open Expect;

[@bs.module "jest-date-mock"] external clear: unit => unit = "clear";
[@bs.module "jest-date-mock"] external advanceTo: float => unit = "advanceTo";

afterEach(clear);

describe("#formatTimestamp", () =>
  test("should format a timestamp", () =>
    expect(Date.Format.timestamp(1544185210000.))
    |> toEqual("2018-12-07")
  )
);

describe("#isFriday", () => {
  test("is it friday", () => {
    advanceTo(1550839500000.0);

    expect(Date.isDay(Friday)) |> toEqual(true);
  });

  test("is it not friday", () => {
    advanceTo(1551335810449.0);

    expect(Date.isDay(Friday)) |> toEqual(false);
  });
});

describe("#isMonth", () => {
  test("is it December", () => {
    advanceTo(1576496351507.0);

    expect(Date.isMonth(December)) |> toEqual(true);
  });

  test("is it not December", () => {
    advanceTo(1573516800000.0); // November

    expect(Date.isMonth(December)) |> toEqual(false);
  });
});
