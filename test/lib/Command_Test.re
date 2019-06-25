open TestFramework;
open Wejay;

describe("Command", ({test, _}) =>
  test("parse", ({expect}) => {
    let payload = command =>
      `Assoc([("command", `String(command)), ("args", `String("hello"))]);

    expect.string(
      payload("search")
      |> Api.decode
      |> (({command, _}) => command |> Command.to_string),
    ).
      toEqual(
      "search",
    );

    expect.string(
      payload("help")
      |> Api.decode
      |> (({command, _}) => command |> Command.to_string),
    ).
      toEqual(
      "help",
    );

    expect.string(
      payload("queue")
      |> Api.decode
      |> (({command, _}) => command |> Command.to_string),
    ).
      toEqual(
      "queue",
    );

    expect.string(
      payload("woot")
      |> Api.decode
      |> (({command, _}) => command |> Command.to_string),
    ).
      toEqual(
      "unknown",
    );
  })
);
