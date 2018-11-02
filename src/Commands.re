type emoji =
  | ThumbsDown
  | ThumbsUp;

type egg =
  | FreeBird
  | Friday
  | IteamClassics
  | Shoreline
  | Slowdance
  | Tequila;

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
  | Toplist
  | Unknown
  | Unmute
  | Volume;

let parseCommand = text =>
  text
  |> Utils.removeUser
  |> Js.String.toLowerCase
  |> Js.String.split(" ")
  |> Js.Array.slice(~start=0, ~end_=1)
  |> (array => array[0]);

let decodeCommand = text =>
  switch (parseCommand(text)) {
  | ":+1:" => Emoji(ThumbsUp)
  | ":-1:" => Emoji(ThumbsDown)
  | "blame" => Blame
  | "clear" => Clear
  | "currentqueue"
  | "getqueue" => CurrentQueue
  | "freebird" => EasterEgg(FreeBird)
  | "friday" => EasterEgg(Friday)
  | "fullqueue" => FullQueue
  | "classics" => EasterEgg(IteamClassics)
  | "shoreline" => EasterEgg(Shoreline)
  | "slowdance" => EasterEgg(Slowdance)
  | "tequila" => EasterEgg(Tequila)
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
  | "toplist" => Toplist
  | "unmute" => Unmute
  | "volume" => Volume
  | _ => Unknown
  };
