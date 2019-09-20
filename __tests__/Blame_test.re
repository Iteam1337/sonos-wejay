open Jest;
open Expect;

test(
  "creates a message when there's no information of who added the track", () => {
  let hits: Elastic.Search.t = [||];

  expect(Blame.message(hits)) |> toMatchSnapshot;
});

test("creates a message when only one user has added the track", () => {
  let hits: Elastic.Search.t = [|
    {sender: "1337", timestamp: "2019-03-22T10:49:13.743Z"},
  |];

  expect(Blame.message(hits)) |> toMatchSnapshot;
});

test("creates a message when multiple people have added a track", () => {
  let hits: Elastic.Search.t = [|
    {sender: "1337", timestamp: "2019-03-22T10:49:13.743Z"},
    {sender: "1338", timestamp: "2019-03-22T10:49:13.743Z"},
  |];

  expect(Blame.message(hits)) |> toMatchSnapshot;
});
