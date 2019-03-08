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

  let freeBird = track("4qsAYBCJnu2OkTKUVbbOF1");
  let friday = track("4fK6E2UywZTJIa5kWnCD6x");
  let shoreline = track("77jVczOFXfbdugN4djsIqs");
  let tequila = track("5gJKsGij5oGt5H5RSFYXPa");
  let worldwideweb = track("597NerobfkV9DhKCySXXWF");
};

module Playlist = {
  open SpotifyUtils;

  let iteamClassics =
    playlist(~user="believer", ~id="445NQ4LkJFtBsHUOdr3LFI");
  let slowdance = playlist(~user="believer", ~id="5DQzhEf0U4Lji5kvXnPYSy");
};

let run = egg => {
  let qAsLast = track => Queue.last(track);
  let qAsNext = track => Queue.next(track);

  switch (egg) {
  | IteamClassics => Playlist.iteamClassics->qAsLast
  | FreeBird => Track.freeBird->qAsNext
  | Friday =>
    Utils.isFriday()
      ? Track.friday->qAsNext
      : Js.Promise.resolve(`Ok("Sorry, it's not Friday"))
  | Shoreline => Track.shoreline->qAsNext
  | Slowdance => Playlist.slowdance->qAsLast
  | Tequila => Track.tequila->qAsNext
  | WWW => Track.worldwideweb->qAsNext
  };
};
