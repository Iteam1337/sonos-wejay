open TestFramework;
open Wejay.Decode.Spotify;

let make_track_mock =
    ((albumName, artist, cover, duration, id, name, releaseDate, uri)) =>
  Ezjsonm.(
    dict([
      ("albumName", string(albumName)),
      ("artist", string(artist)),
      ("cover", string(cover)),
      ("duration", int(duration)),
      ("id", string(id)),
      ("name", string(name)),
      ("releaseDate", string(releaseDate)),
      ("uri", string(uri)),
    ])
  );

let rhcp = (
  "By the Way (Deluxe Edition)",
  "Red Hot Chili Peppers",
  "https://i.scdn.co/image/39577b5613c625bd98e53ba74963be0a98bdd857",
  269000,
  "3ZOEytgrvLwQaqXreDs2Jx",
  "Can't Stop",
  "2002-07-09",
  "spotify:track:3ZOEytgrvLwQaqXreDs2Jx",
);

let miley = (
  "Bangerz (Deluxe Version)",
  "Miley Cyrus",
  "https://i.scdn.co/image/1bbb957cadafd87d859e1de8435a02536ef3a28b",
  231240,
  "2y4lAQpi5VTNLu2ldeTdUH",
  "We Can't Stop",
  "2013-10-04",
  "spotify:track:2y4lAQpi5VTNLu2ldeTdUH",
);

describe("Spotify Decode", ({test, _}) => {
  test("Track.of_json", ({expect}) => {
    let payload = rhcp |> make_track_mock;

    let track = payload |> Track.of_json;

    expect.string(track.artist).toEqual("Red Hot Chili Peppers");
    expect.string(track.albumName).toEqual("By the Way (Deluxe Edition)");
    expect.string(track.cover).toEqual(
      "https://i.scdn.co/image/39577b5613c625bd98e53ba74963be0a98bdd857",
    );
    expect.int(track.duration).toBe(269000);
    expect.string(track.id).toEqual("3ZOEytgrvLwQaqXreDs2Jx");
    expect.string(track.name).toEqual("Can't Stop");
    expect.string(track.releaseDate).toEqual("2002-07-09");
    expect.string(track.uri).toEqual("spotify:track:3ZOEytgrvLwQaqXreDs2Jx");
  });

  test("Tracks.of_json", ({expect}) => {
    let payload =
      Ezjsonm.(dict([("tracks", [rhcp, miley] |> list(make_track_mock))]));

    expect.fn(() => payload |> Tracks.of_json |> List.length).not.toThrow();
  });

  test("Tracks.of_json - when empty", ({expect}) => {
    let payload = Ezjsonm.(dict([("tracks", [] |> list(make_track_mock))]));

    expect.fn(() => payload |> Tracks.of_json |> List.length).not.toThrow();
  });
});
