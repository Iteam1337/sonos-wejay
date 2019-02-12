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

let handleEasterEgg = (egg, user, sendMessage) => {
  let qAsLast = track =>
    Queue.asLast(~track, ~user, ~sendMessage, ()) |> ignore;
  let qAsNext = uri => uri->Queue.asNext(user, sendMessage);

  switch (egg) {
  | IteamClassics => Playlist.iteamClassics->qAsLast
  | FreeBird => Track.freeBird->qAsNext
  | Friday =>
    Utils.isFriday ?
      Track.friday->qAsNext : sendMessage("Sorry, it's not Friday") |> ignore
  | Shoreline => Track.shoreline->qAsNext
  | Slowdance => Playlist.slowdance->qAsLast
  | Tequila => Track.tequila->qAsNext
  | WWW => Track.worldwideweb->qAsNext
  };
};
