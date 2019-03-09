let toPlaylistUri = (~user, ~id) =>
  "spotify:user:" ++ user ++ ":playlist:" ++ id;
let toUri = id => "spotify:track:" ++ id;
let trackId = uri => uri->Utils.splitBy(":") |> (items => items[2]);

type searchType =
  | Album
  | Artist
  | Playlist
  | Track;

let spotifySearchUrl = (~query, ~limit=5, ~market="SE", ~searchType=Track, ()) => {
  let q =
    query
    |> Js.String.replaceByRe([%re "/&amp;/g"], "&")
    |> Js.Global.encodeURIComponent;

  let sType =
    switch (searchType) {
    | Album => "album"
    | Artist => "artist"
    | Playlist => "playlist"
    | Track => "track"
    };

  "https://api.spotify.com/v1/search?q="
  ++ q
  ++ "&type="
  ++ sType
  ++ "&limit="
  ++ string_of_int(limit)
  ++ "&market="
  ++ market;
};

let isSpotifyCopy = text =>
  Js.String.includes("https://open.spotify.com/track", text)
  || Js.String.includes("https://open.spotify.com/user/", text);
