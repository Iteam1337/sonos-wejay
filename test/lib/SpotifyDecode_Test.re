open TestFramework;
open Wejay.Decode.Spotify;

let rhcp =
  Ezjsonm.(
    dict([
      ("albumName", string("By the Way (Deluxe Edition)")),
      ("artist", string("Red Hot Chili Peppers")),
      (
        "cover",
        string(
          "https://i.scdn.co/image/39577b5613c625bd98e53ba74963be0a98bdd857",
        ),
      ),
      ("duration", int(269000)),
      ("id", string("3ZOEytgrvLwQaqXreDs2Jx")),
      ("name", string("Can't Stop")),
      ("releaseDate", string("2002-07-09")),
      ("uri", string("spotify:track:3ZOEytgrvLwQaqXreDs2Jx")),
    ])
  );

let miley =
  Ezjsonm.(
    dict([
      ("albumName", string("Bangerz (Deluxe Version)")),
      ("artist", string("Miley Cyrus")),
      (
        "cover",
        string(
          "https://i.scdn.co/image/1bbb957cadafd87d859e1de8435a02536ef3a28b",
        ),
      ),
      ("duration", int(231240)),
      ("id", string("2y4lAQpi5VTNLu2ldeTdUH")),
      ("name", string("We Can't Stop")),
      ("releaseDate", string("2013-10-04")),
      ("uri", string("spotify:track:2y4lAQpi5VTNLu2ldeTdUH")),
    ])
  );

describe("Spotify Decode", ({test, _}) =>
  test("Track.of_json", ({expect})
    => {
      let payload = rhcp;

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
      expect.string(track.uri).toEqual(
        "spotify:track:3ZOEytgrvLwQaqXreDs2Jx",
      );
    })
    /* test("Tracks.of_json", ({expect}) => { */
    /*   let payload = Ezjsonm.(dict([("tracks", list(rhcp, miley))])); */
    /*   /1* let tracks = payload |> Tracks.of_json; *1/ */
    /*   Console.log(payload); */
    /*   /1* let rhcp = tracks |> List.hd; *1/ */
    /*   /1* let miley = tracks |> List.tl; *1/ */
    /*   expect.string("x").toEqual("RHCP"); */
    /* }); */
);

/* test("Tracks.of_json", ({expect}) => { */
/*   let payload = */
/*     `Assoc([ */
/*       ( */
/*         "tracks", */
/*         `List([ */
/*           `Assoc([ */
/*             ("albumName", `String("By the Way (Deluxe Edition)")), */
/*             ("artist", `String("Red Hot Chili Peppers")), */
/*             ( */
/*               "cover", */
/*               `String( */
/*                 "https://i.scdn.co/image/39577b5613c625bd98e53ba74963be0a98bdd857", */
/*               ), */
/*             ), */
/*             ("duration", `Int32(269000)), */
/*             ("id", `String("3ZOEytgrvLwQaqXreDs2Jx")), */
/*             ("name", `String("Can't Stop")), */
/*             ("releaseDate", `String("2002-07-09")), */
/*             ("uri", `String("spotify:track:3ZOEytgrvLwQaqXreDs2Jx")), */
/*           ]), */
/*           `Assoc([ */
/*             ("albumName", `String("Bangerz (Deluxe Version)")), */
/*             ("artist", `String("Miley Cyrus")), */
/*             ( */
/*               "cover", */
/*               `String( */
/*                 "https://i.scdn.co/image/1bbb957cadafd87d859e1de8435a02536ef3a28b", */
/*               ), */
/*             ), */
/*             ("duration", `Int32(231240)), */
/*             ("id", `String("2y4lAQpi5VTNLu2ldeTdUH")), */
/*             ("name", `String("We Can't Stop")), */
/*             ("releaseDate", `String("2013-10-04")), */
/*             ("uri", `String("spotify:track:2y4lAQpi5VTNLu2ldeTdUH")), */
/*           ]), */
/*         ]), */
/*       ), */
/*     ]); */
/*   /1* let tracks = payload |> Tracks.of_json; *1/ */
/*   Console.log(payload); */
/*   /1* let rhcp = tracks |> List.hd; *1/ */
/*   /1* let miley = tracks |> List.tl; *1/ */
/*   expect.string("x").toEqual("RHCP"); */
/* }) */
