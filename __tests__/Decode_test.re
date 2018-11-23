open Jest;
open Decode;

describe("Decode", () => {
  open Expect;
  
    describe("parseQuery", () => {
      test("removes user", () =>
        expect(parseQuery("<@UD8UR2GGP> s doors end")) |> toEqual("doors end")
      );

      test("handles command query", () =>
        expect(parseQuery("s test")) |> toEqual("test")
      );
    })

    describe("eventPayload", () => {
      test("parses eventPayload", () => {
        let mockEvent = {
          event: {
            subtype: Bot,
            user: Some("UXXXXXXX"),
            command: Commands.Help,
            channel: "AA12345678",
            text: "",
          },
          eventType: EventCallback
        }
        let mockPayload = {|{
          "event": {
              "subtype": "bot_message",
              "user": "UXXXXXXX",
              "text": "help",
              "channel": "AA12345678"
          },
          "type": "event_callback"
        }|} |> Json.parseOrRaise

        let {eventType, event}: Decode.eventPayload = mockPayload |> Decode.eventPayload;
        let {eventType: eType, event: e}: Decode.eventPayload = mockEvent;

        expect((eventType, event)) |> toEqual((eType, e));
      });
    })
});
