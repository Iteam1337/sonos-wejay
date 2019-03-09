open Jest;
open Expect;

test("creates a formatted message", () => {
  let tracks: array(Spotify.Track.t) = [|
    {
      album: {
        images: [||],
        name: "30 Seconds to Mars",
        releaseDate: "2000",
      },
      artists: [|{name: "30 Seconds to Mars"}|],
      duration: 3600,
      name: "Echelon",
      uri: "uri",
    },
    {
      album: {
        images: [||],
        name: "There's a hell...",
        releaseDate: "2000",
      },
      artists: [|{name: "Bring me the Horizon"}|],
      duration: 3600,
      name: "Hospital for Souls",
      uri: "uri",
    },
  |];

  let hits: array(Elastic.Aggregate.t) = [|
    {key: "", count: 2},
    {key: "", count: 1},
  |];

  expect(MostPlayed.message(tracks, hits)) |> toMatchSnapshot;
});
