open Jest;
open Messages;
open Expect;

test("has text for when nothing is playing", () =>
  expect(nothingIsPlaying) |> toMatchSnapshot
);

test("has text for when queue is empty", () =>
  expect(emptyQueue) |> toMatchSnapshot
);

test("has help text", () =>
  expect(help) |> toMatchSnapshot
);

test("has help text for unknown command", () =>
  expect(unknownCommand("some_command_that_should_not_exist"))
  |> toMatchSnapshot
);

test("has help text for command `time`", () =>
  expect(thisIsWejay) |> toMatchSnapshot
);
