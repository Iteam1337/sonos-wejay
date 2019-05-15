let toPlaylistUri = (~user, ~id) => {j|spotify:user:$user:playlist:$id|j};
let toUri = id => {j|spotify:track:$id|j};
let trackId = uri => uri->Utils.splitBy(":") |> (items => items[2]);

type searchType =
  | Album
  | Artist
  | Playlist
  | Track;

let typeOfSearch =
  fun
  | Album => "album"
  | Artist => "artist"
  | Playlist => "playlist"
  | Track => "track";

let isSpotifyCopy = text =>
  Js.String.includes("https://open.spotify.com/track", text)
  || Js.String.includes("https://open.spotify.com/user/", text);
