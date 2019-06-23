let handle = ({command, args}: Api.t) => {
  let%lwt payload =
    switch (command) {
    | Search => Spotify.search(args)
    | _ => Spotify.search(args)
    };
  Lwt.return((command, payload));
};
