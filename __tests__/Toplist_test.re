open Jest;
open Expect;

test("creates a message when there are no plays", () => {
  let hits: Elastic.Aggregate.t = [||];

  expect(Toplist.message(hits)) |> toMatchSnapshot;
});

test("creates a messages when plays exist", () => {
  let hits: Elastic.Aggregate.t = [|
    {key: "1337", count: 5},
    {key: "1338", count: 3},
  |];

  expect(Toplist.message(hits)) |> toMatchSnapshot;
});
