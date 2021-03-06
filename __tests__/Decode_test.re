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
      let mockEvent =
        Decode.EventType.Event(
          Some({
            user: Some("UXXXXXXX"),
            command: Human(Commands.Help),
            channel: "AA12345678",
            text: "",
          }),
        );

      let mockPayload =
        {|{
          "event": {
              "user": "UXXXXXXX",
              "text": "help",
              "channel": "AA12345678"
          },
          "type": "event_callback"
        }|}
        |> Json.parseOrRaise;

      mockPayload |> Decode.EventResponse.make |> expect |> toEqual(mockEvent);
    })
  );
});
