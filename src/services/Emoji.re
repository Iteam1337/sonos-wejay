type t =
  | ThumbsDown
  | ThumbsUp
  | Santa
  | UnhandledEmoji(string);

module Christmas = {
  open SpotifyUtils;

  let songs = [|
    /* Andy Williams - It's the Most Wonderful Time of the Year */
    "5hslUAKq9I9CG2bAulFkHN",
    /* Ariana Grande - Santa Tell Me */
    "0lizgQ7Qw35od7CYaoMBZb",
    /* Daryl Hall & John Oates - Jingle Bell Rock */
    "3y8Z2sOJrBt6yKEfsZG1NY",
    /* Frank Sinatra - Let it Snow */
    "7inXu0Eaeg02VsM8kHNvzM",
    /* José Feliciano - Feliz Navidad */
    "7xQmVmSfCt4LD9H2uFoWOO",
    /* Justin Bieber - Mistletoe */
    "7xapw9Oy21WpfEcib2ErSA",
    /* Mariah Carey - All I Want for Christmas Is You */
    "0bYg9bo50gSsH3LtXe2SQn",
    /* Mariah Carey - Santa Claus Is Comin' To Town */
    "1f2lkuLldqRzvaFjJdjGXX",
    /* Michael Bublé - It's Beginning To Look A Lot Like Christmas */
    "4Dgp0jxrJdpKrZfVmgxekz",
    /* Triad - Tänd ett ljus */
    "7ogTJKHkj8IMxjhItiFKRC",
    /* Wham! - Last Christmas */
    "77nF1t4qesuOJRd8lbIzNX",
  |];

  let randomTrack = () => {
    let randomSongIndex = Random.int(Belt.Array.length(songs));
    let songId = songs[randomSongIndex];
    track(songId);
  };
};

let emojiCommand = text => {
  let cleanEmoji = text->Regex.replaceByRe(Regex.Patterns.skinTone, "");

  switch (cleanEmoji) {
  | ":thumbsup:"
  | ":+1:" => ThumbsUp
  | ":thumbsdown:"
  | ":-1:" => ThumbsDown
  | ":santa:" => Santa
  | emoji => UnhandledEmoji(emoji)
  };
};

let handleEmoji = (emoji, sendMessage) =>
  switch (emoji) {
  | ThumbsDown => Volume.updateVolumeWithValue(-10., sendMessage)
  | ThumbsUp => Volume.updateVolumeWithValue(10., sendMessage)
  | Santa => Christmas.randomTrack()->Queue.asNext(None, sendMessage)
  | UnhandledEmoji(emoji) => sendMessage(Messages.unknownCommand(emoji))
  };

let isEmoji = command =>
  switch (Js.String.match(Regex.Patterns.isEmoji, command)) {
  | None => false
  | Some(_) => true
  };
