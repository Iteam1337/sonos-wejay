open Jest;

describe("Services", () =>
  Expect.(
    describe("#nowPlayingData", () => {
      test("should handle track position", () =>
        expect(
          Services.nowPlayingData({
            artist: "Bring Me The Horizon",
            albumArtURI: "",
            albumArtURL: "",
            position: 10.,
            uri: "",
            title: "Go to Hell, for Heaven's Sake",
            album: Some("Sempiternal"),
            duration: 243.,
            queuePosition: 1.,
          }),
        )
        |> toEqual(
             "*Currently playing*\nBring Me The Horizon - Go to Hell, for Heaven's Sake (Sempiternal)\nPosition in queue 1 - 0:10/4:02",
           )
      );

      test("should handle empty data", () =>
        expect(
          Services.nowPlayingData({
            artist: "",
            albumArtURI: "",
            albumArtURL: "",
            position: 0.,
            uri: "",
            title: "",
            album: None,
            duration: 0.,
            queuePosition: 0.,
          }),
        )
        |> toEqual(
             "Nothing is currently playing, add a track using `search <your track>`",
           )
      );
    })
  )
);
