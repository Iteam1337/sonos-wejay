let playlist = (~user, ~id) => "spotify:user:" ++ user ++ ":playlist:" ++ id;
let track = id => "spotify:track:" ++ id;

module Tracks = {
  let freeBird = track("4qsAYBCJnu2OkTKUVbbOF1");
  let friday = track("4fK6E2UywZTJIa5kWnCD6x");
  let shoreline = track("77jVczOFXfbdugN4djsIqs");
  let tequila = track("5gJKsGij5oGt5H5RSFYXPa");
  let worldwideweb = track("597NerobfkV9DhKCySXXWF");

  module Christmas = {
    let songs = [|
      ("Justin Bieber - Mistletoe", "7xapw9Oy21WpfEcib2ErSA"),
      ("Ariana Grande - Santa Tell Me", "0lizgQ7Qw35od7CYaoMBZb"),
      ("Frank Sinatra - Let it Snow", "7inXu0Eaeg02VsM8kHNvzM"),
      ("Triad - Tänd ett ljus", "7ogTJKHkj8IMxjhItiFKRC"),
      ("José Feliciano - Feliz Navidad", "7xQmVmSfCt4LD9H2uFoWOO"),
      ("Wham! - Last Christmas", "77nF1t4qesuOJRd8lbIzNX"),
      (
        "Mariah Carey - All I Want for Christmas Is You",
        "0bYg9bo50gSsH3LtXe2SQn",
      ),
      (
        "Daryl Hall & John Oates - Jingle Bell Rock",
        "3y8Z2sOJrBt6yKEfsZG1NY",
      ),
      (
        "Mariah Carey - Santa Claus Is Comin' To Town",
        "1f2lkuLldqRzvaFjJdjGXX",
      ),
      (
        "Michael Bublé - It's Beginning To Look A Lot Like Christmas",
        "4Dgp0jxrJdpKrZfVmgxekz",
      ),
      (
        "Andy Williams - It's the Most Wonderful Time of the Year",
        "5hslUAKq9I9CG2bAulFkHN",
      ),
    |];

    let getSong = () => {
      let randomSongIndex = Random.int(ArrayLabels.length(songs));
      let (_, songId) = songs[randomSongIndex];
      track(songId);
    };
  };
};

module Playlists = {
  let iteamClassics =
    playlist(~user="believer", ~id="445NQ4LkJFtBsHUOdr3LFI");
  let slowdance = playlist(~user="believer", ~id="5DQzhEf0U4Lji5kvXnPYSy");
};

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
