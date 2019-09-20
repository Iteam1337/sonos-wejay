type t =
  | FreeBird
  | Friday
  | IteamClassics
  | Rednex
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
  let rednex = toUri("06hsdMbBxWGqBO0TV0Zrkf");

  let easterEggTracks = [
    freeBird,
    friday,
    shoreline,
    tequila,
    worldwideweb,
    rednex,
  ];
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
    | Rednex => Track.rednex->next
    | Shoreline => Track.shoreline->next
    | Slowdance => Playlist.slowdance->last
    | Tequila => Track.tequila->next
    | WWW => Track.worldwideweb->next
  );

let isEasterEgg = () => {
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(({uri}: Sonos.Decode.currentTrackResponse) =>
         Track.easterEggTracks
         ->Belt.List.some(track => track == Utils.sonosUriToSpotifyUri(uri))
         ->resolve
       )
  );
};