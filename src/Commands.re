type egg =
  | FreeBird
  | Friday
  | IteamClassics
  | Shoreline
  | Slowdance
  | Tequila;

type command =
  | Clear
  | CurrentQueue
  | EasterEgg(egg)
  | Help
  | Library
  | Mute
  | Next
  | NowPlaying
  | Pause
  | Play
  | Previous
  | Queue
  | Search
  | Unknown
  | Unmute
  | Volume;

let decodeCommand = text =>
  switch (
    text
    |> Js.String.toLowerCase
    |> Js.String.split(" ")
    |> Js.Array.slice(~start=0, ~end_=1)
    |> (array => array[0])
  ) {
  | "clear" => Clear
  | "currentqueue"
  | "getqueue" => CurrentQueue
  | "freebird" => EasterEgg(FreeBird)
  | "friday" => EasterEgg(Friday)
  | "classics" => EasterEgg(IteamClassics)
  | "shoreline" => EasterEgg(Shoreline)
  | "slowdance" => EasterEgg(Slowdance)
  | "tequila" => EasterEgg(Tequila)
  | "help" => Help
  | "l"
  | "library" => Library
  | "mute" => Mute
  | "next" => Next
  | "np"
  | "nowplaying" => NowPlaying
  | "pause" => Pause
  | "play" => Play
  | "previous" => Previous
  | "q"
  | "queue" => Queue
  | "s"
  | "search" => Search
  | "unmute" => Unmute
  | "volume" => Volume
  | _ => Unknown
  };
