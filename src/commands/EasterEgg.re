type t =
  | FreeBird
  | Friday
  | IteamClassics
  | Shoreline
  | Slowdance
  | Tequila
  | WWW;

module Track = {
  open SpotifyUtils;

  let freeBird = toUri("4qsAYBCJnu2OkTKUVbbOF1");
  let friday = toUri("4fK6E2UywZTJIa5kWnCD6x");
  let shoreline = toUri("77jVczOFXfbdugN4djsIqs");
  let tequila = toUri("5gJKsGij5oGt5H5RSFYXPa");
  let worldwideweb = toUri("597NerobfkV9DhKCySXXWF");
};

module Playlist = {
  open SpotifyUtils;

  let iteamClassics =
    toPlaylistUri(~user="believer", ~id="445NQ4LkJFtBsHUOdr3LFI");
  let slowdance =
    toPlaylistUri(~user="believer", ~id="5DQzhEf0U4Lji5kvXnPYSy");
};

let run =
  Queue.(
    fun
    | IteamClassics => Playlist.iteamClassics->last
    | FreeBird => Track.freeBird->next
    | Friday =>
      Utils.isFriday()
        ? Track.friday->next
        : Js.Promise.resolve(`Ok("Sorry, it's not Friday"))
    | Shoreline => Track.shoreline->next
    | Slowdance => Playlist.slowdance->last
    | Tequila => Track.tequila->next
    | WWW => Track.worldwideweb->next
  );
