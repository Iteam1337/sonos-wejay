open Jest;
open Expect;

describe("#formatTimestamp", () =>
  test("should format a timestamp", () =>
    expect(DateFns.Format.timestamp(1544185210000.))
    |> toEqual("2018-12-07")
  )
);
