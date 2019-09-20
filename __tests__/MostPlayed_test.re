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

describe("#filterPlaylists", () => {
  test("should remove spotify playlists", () => {
    let test: Elastic.Aggregate.aggregate = {
      key: "spotify:user:spotify:playlist:37i9dQZF1DX9uKNf5jGX6m",
      count: 3,
    };

    expect(MostPlayed.filterPlaylists(test)) |> toBe(false);
  });

  test("should allow normal tracks", () => {
    let test: Elastic.Aggregate.aggregate = {
      key: "spotify:track:3Bj2mrlp3tALHO5U3mK8zM",
      count: 3,
    };

    expect(MostPlayed.filterPlaylists(test)) |> toBe(true);
  });
});
