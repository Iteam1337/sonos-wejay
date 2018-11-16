let playlist = (~user, ~id) => "spotify:user:" ++ user ++ ":playlist:" ++ id;
let track = id => "spotify:track:" ++ id;

module Tracks = {
  let freeBird = track("4qsAYBCJnu2OkTKUVbbOF1");
  let friday = track("4fK6E2UywZTJIa5kWnCD6x");
  let shoreline = track("77jVczOFXfbdugN4djsIqs");
  let tequila = track("5gJKsGij5oGt5H5RSFYXPa");

  module Xmas = {
    let xmasSongs = [|
      ("Justin Bieber - Mistletoe", "7xapw9Oy21WpfEcib2ErSA"),
      ("Ariana Grande - Santa Tell Me", "0lizgQ7Qw35od7CYaoMBZb"),
      ("Frank Sinatra - Let it Snow", "7inXu0Eaeg02VsM8kHNvzM"),
      ("Mariah Carey - All I Want for Christmas Is You", "0bYg9bo50gSsH3LtXe2SQn"),
      ("Wham! - Last Christmas", "77nF1t4qesuOJRd8lbIzNX"),
      ("Daryl Hall & John Oates - Jingle Bell Rock", "3y8Z2sOJrBt6yKEfsZG1NY"),
      ("Feliz Navidad", "7xQmVmSfCt4LD9H2uFoWOO"),
      ("Mariah Carey - Santa Claus Is Comin' To Town", "1f2lkuLldqRzvaFjJdjGXX")
    |];

    let getXmasSong = () => {
      let randomSongIndex = Random.int(ArrayLabels.length(xmasSongs))
      let (_, songUri) = Array.get(xmasSongs, randomSongIndex)
      track(songUri)
    }
  }
};

module Playlists = {
  let iteamClassics =
    playlist(~user="believer", ~id="445NQ4LkJFtBsHUOdr3LFI");
  let slowdance = playlist(~user="believer", ~id="5DQzhEf0U4Lji5kvXnPYSy");
};
