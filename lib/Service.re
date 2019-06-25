type t = [
  | `Track(Decode.Spotify.Track.t)
  | `Tracks(list(Decode.Spotify.Tracks.t))
  | `String(string)
];

let handle = ({command, args}: Api.t) => {
  let%lwt payload =
    switch (command) {
    | Search => Spotify.search(args)
    | Queue => Spotify.track(args)
    | Help => Ok(`String("Help")) |> Lwt.return
    | Unknown => Ok(`String("Unknown")) |> Lwt.return
    };

  Lwt.return((command, payload));
};
