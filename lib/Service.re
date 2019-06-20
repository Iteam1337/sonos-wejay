let handle = ({command, args}: Api.t) => {
  switch (command) {
  | Search => Spotify.search(args)
  | _ => Spotify.search(args)
  };
};
