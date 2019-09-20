open Jest;
open Utils;
open Expect;

[@bs.module "jest-date-mock"] external clear: unit => unit = "clear";
[@bs.module "jest-date-mock"] external advanceTo: float => unit = "advanceTo";

afterEach(clear);

describe("#formatTimestamp", () =>
  test("should format a timestamp", () =>
    expect(formatTimestamp(1544185210000.)) |> toEqual("2018-12-07")
  )
);

describe("#artistAndTitle", () =>
  test("artist and title combined", () =>
    expect(
      artistAndTitle(
        ~artist="Bring Me The Horizon",
        ~title="Hospital for Souls",
      ),
    )
    |> toEqual("Bring Me The Horizon - Hospital for Souls")
  )
);

describe("#listNumber", () =>
  test("create a list number item", () =>
    expect(listNumber(0)) |> toEqual("1. ")
  )
);

describe("#parseDuration", () =>
  test("parses durations as seconds", () =>
    expect(parseDuration(50.)) |> toEqual("0:50")
  )
);

describe("#parsedTrack", () => {
  test("parses Spotify URI", () =>
    expect(parsedTrack("spotify:track:4UQLQJu3DNvVkMVglwElU2"))
    |> toEqual("spotify:track:4UQLQJu3DNvVkMVglwElU2")
  );

  test("parses Spotify URI with Slack brackets", () =>
    expect(parsedTrack("<spotify:track:4UQLQJu3DNvVkMVglwElU2>"))
    |> toEqual("spotify:track:4UQLQJu3DNvVkMVglwElU2")
  );

  test("parses Spotify URL", () =>
    expect(
      parsedTrack(
        "https://open.spotify.com/track/6DUdjPhPTpE3zMTA9jdCex?si=pufHMXrGTpC89VBP88CAow",
      ),
    )
    |> toEqual("spotify:track:6DUdjPhPTpE3zMTA9jdCex")
  );
});

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

describe("#parseSpotifyCopy", () => {
  test("should parse spotify copy/paste", () =>
    expect(
      parseSpotifyCopy(
        "<https://open.spotify.com/track/2mPE346iPVnss0GoRgwGTK>\n<https://open.spotify.com/track/4LT7xQZgICM6FqUJkWI6aM>",
      ),
    )
    |> toEqual([|
         "spotify:track:2mPE346iPVnss0GoRgwGTK",
         "spotify:track:4LT7xQZgICM6FqUJkWI6aM",
       |])
  );

  test("should parse spotify playlist links", () =>
    expect(
      parseSpotifyCopy(
        "https://open.spotify.com/user/believer/playlist/77ffhasgb5saaregvpnxwd?si=e-l_i0jdt7oq370tn23ccg",
      ),
    )
    |> toEqual([|"spotify:user:believer:playlist:77ffhasgb5saaregvpnxwd"|])
  );
});

test("is it friday", () => {
  advanceTo(1550839500000.0);

  expect(isFriday()) |> toEqual(true);
});

test("is it not friday", () => {
  advanceTo(1551335810449.0);

  expect(isFriday()) |> toEqual(false);
});
