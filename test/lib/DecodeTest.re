open TestFramework;

describe("Decode", ({test, _}) => {
  test("decode_event", ({expect}) => {
    let payload =
      Ezjsonm.(
        dict([
          ("text", string("search rage against the machine")),
          ("channel", string("AA12345678")),
          ("user", string("UD8UR2GGP")),
          ("subtype", string("bot_message")),
        ])
      );

    expect.fn(() => Wejay.Decode.Slack.Event.of_json(payload)).not.toThrow();
  });

  test("decode_command", ({expect}) => {
    let query = "search rage against the machine";
    let (command, args) = Wejay.Decode.Slack.Event.decode_command(query);

    expect.string(Wejay.Command.(command |> to_string)).toEqual("search");

    expect.string(args).toEqual("rage against the machine");
  });
});
