let playlist = (~user, ~id) => "spotify:user:" ++ user ++ ":playlist:" ++ id;
let track = id => "spotify:track:" ++ id;

module Tracks = {
  let freeBird = track("4qsAYBCJnu2OkTKUVbbOF1");
  let friday = track("4fK6E2UywZTJIa5kWnCD6x");
  let shoreline = track("77jVczOFXfbdugN4djsIqs");
  let tequila = track("5gJKsGij5oGt5H5RSFYXPa");
};

module Playlists = {
  let iteamClassics =
    playlist(~user="believer", ~id="445NQ4LkJFtBsHUOdr3LFI");
  let slowdance = playlist(~user="believer", ~id="5DQzhEf0U4Lji5kvXnPYSy");
};
