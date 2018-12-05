open Jest;

describe("Queue", () =>
  Expect.(
    describe("#SonosDecode", () =>
      describe("#trackPosition", () =>
        test("should handle track position", () =>
          expect(Queue.trackPosition(~first="1", ~queueAt=1., ()))
          |> toEqual("1")
        )
      )
    )
  )
);
