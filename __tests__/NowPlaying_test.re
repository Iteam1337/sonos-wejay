open Jest;
open Expect;

describe("#nowPlayingData", () => {
  testPromise("nothing is playing", () => {
    let sonos: Sonos.Decode.CurrentTrack.t = {
      album: Some("30 Seconds to Mars"),
      albumArtURI: "",
      albumArtURL: "",
      uri: Some(""),
      artist: "30 Seconds to Mars",
      duration: 3600.0,
      title: "Echelon",
      queuePosition: 0.0,
      position: 1200.0,
    };

    NowPlaying.message(~sonos, ~cover="img")
    |> Js.Promise.then_(v =>
         expect(v) |> toMatchSnapshot |> Js.Promise.resolve
       );
  });

  testPromise("current track", () => {
    let sonos: Sonos.Decode.CurrentTrack.t = {
      album: Some("30 Seconds to Mars"),
      albumArtURI: "",
      albumArtURL: "",
      uri: Some(""),
      artist: "30 Seconds to Mars",
      duration: 360.0,
      title: "Echelon",
      queuePosition: 1.0,
      position: 120.0,
    };

    NowPlaying.message(~sonos, ~cover="img")
    |> Js.Promise.then_(v =>
         expect(v) |> toMatchSnapshot |> Js.Promise.resolve
       );
  });
});
