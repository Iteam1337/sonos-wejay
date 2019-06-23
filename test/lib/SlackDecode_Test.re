open TestFramework;
open Wejay.Decode.Slack.Event;

describe("Slack Decode", ({test, _}) => {
  test("decode_event_payload", ({expect}) => {
    let payload =
      Ezjsonm.(
        dict([
          ("token", string("XXYYZZ")),
          ("team_id", string("TXXXXXXXX")),
          ("api_app_id", string("AXXXXXXXXX")),
          (
            "event",
            dict([
              ("text", string("search hello")),
              ("channel", string("1234567890.123456")),
              ("user", string("UXXXXXXX1")),
              ("type", string("xyz")),
            ]),
          ),
          ("type", string("event_callback")),
          ("authed_users", list(string, ["UXXXXXXX1", "UXXXXXXX2"])),
          ("event_id", string("Ev08MFMKH6")),
          ("event_time", int(1234567890)),
        ])
      );

    let {event, eventType} = payload |> event_of_json;

    expect.same(eventType, EventCallback);
    expect.option(Some(event)).toBeSome();
  });

  test("decode_event", ({expect}) => {
    let payload =
      {|
        {
          "text": "search rage against the machine",
          "channel": "AA12345678",
          "user": "UD8UR2GGP",
          "type": "other"
        }
      |}
      |> Ezjsonm.from_string;

    let event = payload |> of_json;

    expect.string(event.user).toEqual("UD8UR2GGP");
    expect.string(event.channel).toEqual("AA12345678");
    expect.string(event.command.args).toEqual("rage against the machine");
    expect.same(event.subtype, Human);
  });

  test("url_verification", ({expect}) => {
    let payload =
      {|
        {
          "token": "Jhj5dZrVaK7ZwHHjRyZWjbDl",
          "challenge":
        "3eZbrw1aBm2rZgRNFdxV2595E9CY3gmdALWMmHkvFXO7tYXAYM8P",
          "type": "url_verification"
        }
      |}
      |> Ezjsonm.from_string;

    let {challenge} = payload |> decode_verification;

    expect.string(challenge).toEqual(
      "3eZbrw1aBm2rZgRNFdxV2595E9CY3gmdALWMmHkvFXO7tYXAYM8P",
    );
  });

  test("decode_command", ({expect}) => {
    let query = "search rage against the machine";
    let (command, args) = decode_command(query);

    expect.same(command, Wejay.Command.(Search));
    expect.string(args).toEqual("rage against the machine");
  });
});
