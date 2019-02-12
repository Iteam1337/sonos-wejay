type t =
  | ThumbsDown
  | ThumbsUp
  | Santa;

module Christmas = {
  open SpotifyUtils;

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
    ("Daryl Hall & John Oates - Jingle Bell Rock", "3y8Z2sOJrBt6yKEfsZG1NY"),
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

let handleEmoji = (emoji, sendMessage) =>
  switch (emoji) {
  | ThumbsDown => Volume.updateVolumeWithValue(-10., sendMessage)
  | ThumbsUp => Volume.updateVolumeWithValue(10., sendMessage)
  | Santa => Christmas.getSong()->Queue.asNext(None, sendMessage)
  };
