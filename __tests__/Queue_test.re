open Jest;
open Expect;

describe("#SonosDecode", () =>
  describe("#trackPosition", () =>
    test("should handle track position", () =>
      expect(Queue.trackPosition(~first="1", ~queueAt=1., ()))
      |> toEqual("1")
    )
  )
)
describe("#handleRemoveArgs", () => {
  test("no args", () => {
    expect(Queue.handleRemoveArgs(""))
    |> toEqual((1, 1))
  })
  test("returns 1 param correctly", () => {
    expect(Queue.handleRemoveArgs("9"))
    |> toEqual((9, 1))
  })
  test("returns 2 params correctly", () => {
    expect(Queue.handleRemoveArgs("3 3"))
    |> toEqual((3, 3))
  })
  test("returns 2 params correctly even though you add more", () => {
    expect(Queue.handleRemoveArgs("3 3 jimmyärsnygg"))
    |> toEqual((3, 3))
  })
})
