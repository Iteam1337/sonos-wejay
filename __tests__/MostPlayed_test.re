open Jest;
open Expect;

test("creates a formatted message", () => {
  let tracks: array(Spotify.WejayTrack.t) = [|
    {
      albumName: "30 Seconds to Mars",
      artist: "30 Seconds to Mars",
      cover: "",
      duration: 3600.0,
      id: "1",
      name: "Echelon",
      releaseDate: "",
      uri: "spotify:track:uri",
    },
    {
      albumName: "There's a hell...",
      artist: "Bring me the Horizon",
      cover: "",
      duration: 3600.0,
      id: "2",
      name: "Hospital for Souls",
      releaseDate: "",
      uri: "spotify:track:uri",
    },
  |];

  let hits: Elastic.Aggregate.t = [|
    {key: "", count: 2},
    {key: "", count: 1},
  |];

  expect(MostPlayed.message(tracks, hits)) |> toMatchSnapshot;
});
