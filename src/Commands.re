type emoji =
  | ThumbsDown
  | ThumbsUp
  | Santa;

type egg =
  | FreeBird
  | Friday
  | IteamClassics
  | Shoreline
  | Slowdance
  | Tequila
  | WWW;

type command =
  | Blame
  | Clear
  | CurrentQueue
  | EasterEgg(egg)
  | Emoji(emoji)
  | FullQueue
  | Help
  | Library
  | MostPlayed
  | Mute
  | Next
  | NowPlaying
  | Pause
  | Play
  | PlayTrack
  | Previous
  | Queue
  | Search
  | SpotifyCopy(array(string))
  | Time
  | Toplist
  | UnhandledCommand
  | UnknownCommand(string)
  | Unmute
  | Volume;

let parseCommand = text =>
  text
  |> Utils.removeUser
  |> Js.String.toLowerCase
  |> Js.String.split(" ")
  |> Js.Array.slice(~start=0, ~end_=1)
  |> (array => array[0]);

let decodeCommand = text => {
  let isSpotifyCopy =
    Js.String.includes("https://open.spotify.com/track", text)
    || Js.String.includes("https://open.spotify.com/user/", text);

  isSpotifyCopy ?
    SpotifyCopy(Utils.parseSpotifyCopy(text)) :
    (
      switch (parseCommand(text)) {
      | "" => UnhandledCommand
      | ":thumbsup:"
      | ":+1:" => Emoji(ThumbsUp)
      | ":thumbsdown:"
      | ":-1:" => Emoji(ThumbsDown)
      | ":santa:" => Emoji(Santa)
      | "blame" => Blame
      | "classics" => EasterEgg(IteamClassics)
      | "clear" => Clear
      | "cq"
      | "currentqueue"
      | "gq"
      | "getqueue" => CurrentQueue
      | "freebird" => EasterEgg(FreeBird)
      | "friday" => EasterEgg(Friday)
      | "fq"
      | "fullqueue" => FullQueue
      | "help" => Help
      | "l"
      | "library" => Library
      | "mute" => Mute
      | "mostplayed" => MostPlayed
      | "next" => Next
      | "np"
      | "nowplaying" => NowPlaying
      | "pause" => Pause
      | "play" => Play
      | "playtrack" => PlayTrack
      | "previous" => Previous
      | "q"
      | "queue" => Queue
      | "s"
      | "search" => Search
      | "shoreline" => EasterEgg(Shoreline)
      | "slowdance" => EasterEgg(Slowdance)
      | "time" => Time
      | "tequila" => EasterEgg(Tequila)
      | "toplist" => Toplist
      | "unmute" => Unmute
      | "volume" => Volume
      | "www"
      | "worldwideweb" => EasterEgg(WWW)
      | text => UnknownCommand(text)
      }
    );
};

let commandToString = command =>
  switch (command) {
  | Blame => "blame"
  | Clear => "clear"
  | CurrentQueue => "current-queue"
  | EasterEgg(egg) =>
    "easteregg-"
    ++ (
      switch (egg) {
      | IteamClassics => "classics"
      | FreeBird => "freebird"
      | Friday => "friday"
      | Shoreline => "shoreline"
      | Slowdance => "slowdance"
      | Tequila => "tequila"
      | WWW => "world-wide-web"
      }
    )
  | Emoji(_) => "emoji"
  | FullQueue => "full-queue"
  | Help => "help"
  | Library => "library"
  | MostPlayed => "most-played"
  | Mute => "playback-mute"
  | Next => "playback-next"
  | NowPlaying => "now-playing"
  | Pause => "playback-pause"
  | Play => "playback-play"
  | PlayTrack => "playback-playtrack"
  | Previous => "playback-previous"
  | Queue => "queue"
  | SpotifyCopy(_) => "spotify-copy"
  | Search => "search"
  | Time => "time"
  | Toplist => "toplist"
  | UnknownCommand(c) => "unknown-command" ++ c
  | UnhandledCommand => "unhandled-command"
  | Unmute => "playback-unmute"
  | Volume => "volume"
  };
