open Jest;
open Utils;

describe("Utils", () => {
  open Expect;
  describe("#trackInfo", () =>
    test("return artist and track", () =>
      expect(
        trackInfo({
          "artist": "Bring Me The Horizon",
          "title": "Hospital for Souls",
        }),
      )
      |> toEqual("Bring Me The Horizon - Hospital for Souls")
    )
  );

  describe("#parseDuration", () =>
    test("parses durations as seconds", () =>
      expect(parseDuration(50.)) |> toEqual("0:50")
    )
  );

  describe("#cleanFloat", () =>
    test("cleans up a float to string", () =>
      expect(cleanFloat(50.)) |> toEqual("50")
    )
  );

  describe("#createAttachment", () => {
    test("creates attachment without thumb_url", () =>
      expect(createAttachment(~text="Text", ~uri="spotify:track:123", ()))
      |> toEqual({
           "text": "Text",
           "callback_id": "queue",
           "thumb_url": "",
           "actions": [|
             {
               "name": "track",
               "text": "Queue",
               "type": "button",
               "value": "spotify:track:123",
             },
           |],
         })
    );

    test("creates attachment with thumb_url", () =>
      expect(
        createAttachment(
          ~text="Text",
          ~uri="spotify:track:123",
          ~thumbUrl="image.png",
          (),
        ),
      )
      |> toEqual({
           "text": "Text",
           "callback_id": "queue",
           "thumb_url": "image.png",
           "actions": [|
             {
               "name": "track",
               "text": "Queue",
               "type": "button",
               "value": "spotify:track:123",
             },
           |],
         })
    );
  });
});
