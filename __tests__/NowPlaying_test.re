open Jest;
open Expect;

describe("#nowPlayingData", () => {
  test("nothing is playing", () => {
    let sonos: Sonos.Decode.currentTrackResponse = {
      album: Some("30 Seconds to Mars"),
      albumArtURI: "",
      albumArtURL: "",
      uri: "",
      artist: "30 Seconds to Mars",
      duration: 3600.0,
      title: "Echelon",
      queuePosition: 0.0,
      position: 1200.0,
    };

    expect(NowPlaying.message(~sonos, ~cover="img")) |> toMatchSnapshot;
  });

  test("current track", () => {
    let sonos: Sonos.Decode.currentTrackResponse = {
      album: Some("30 Seconds to Mars"),
      albumArtURI: "",
      albumArtURL: "",
      uri: "",
      artist: "30 Seconds to Mars",
      duration: 360.0,
      title: "Echelon",
      queuePosition: 1.0,
      position: 120.0,
    };

    expect(NowPlaying.message(~sonos, ~cover="img")) |> toMatchSnapshot;
  });
});
