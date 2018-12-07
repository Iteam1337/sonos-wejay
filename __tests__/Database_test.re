open Jest;

describe("Database", () =>
  Expect.(
    describe("#formatTimestamp", () =>
      test("should format a timestamp", () =>
        expect(Database.formatTimestamp(1544185210000.))
        |> toEqual("2018-12-07")
      )
    )
  )
);
