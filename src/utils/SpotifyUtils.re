let toPlaylistUri = (~user, ~id) => {j|spotify:user:$user:playlist:$id|j};
let toUri = id => {j|spotify:track:$id|j};
let trackId = uri => uri->Utils.splitBy(":") |> (items => items[2]);

type searchType =
  | Album
  | Artist
  | Playlist
  | Track;

let spotifySearchUrl = (~query, ~limit=5, ~market="SE", ~searchType=Track, ()) => {
  let l = limit->string_of_int;
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

  {j|https://api.spotify.com/v1/search?q=$q&type=$sType&limit=$l&market=$market|j};
};

let isSpotifyCopy = text =>
  Js.String.includes("https://open.spotify.com/track", text)
  || Js.String.includes("https://open.spotify.com/user/", text);
