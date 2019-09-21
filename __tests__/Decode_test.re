open Jest;

describe("Decode", () => {
  open Expect;

  describe("Event.parseText", () => {
    test("removes user", () =>
      expect(Decode.Event.parseText("<@UD8UR2GGP> s doors end"))
      |> toEqual("doors end")
    );

    test("handles command query", () =>
      expect(Decode.Event.parseText("s test")) |> toEqual("test")
    );
  });

  describe("eventPayload", () =>
    test("parses eventPayload", () => {
      let mockEvent: Decode.EventResponse.t = {
        event:
          Some({
            subtype: Bot,
            user: Some("UXXXXXXX"),
            command: Commands.Help,
            channel: "AA12345678",
            text: "",
          }),
        eventType: EventCallback,
      };

      let mockPayload =
        {|{
          "event": {
              "subtype": "bot_message",
              "user": "UXXXXXXX",
              "text": "help",
              "channel": "AA12345678"
          },
          "type": "event_callback"
        }|}
        |> Json.parseOrRaise;

      let {eventType, event}: Decode.EventResponse.t =
        mockPayload |> Decode.EventResponse.make;
      let {eventType: eType, event: e}: Decode.EventResponse.t = mockEvent;

      expect((eventType, event)) |> toEqual((eType, e));
    })
  );
});
