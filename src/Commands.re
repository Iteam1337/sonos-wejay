type t =
  | Blame
  | Clear
  | CurrentQueue
  | EasterEgg(EasterEgg.t)
  | Emoji(Emoji.t)
  | FullQueue
  | Help
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
  | RemoveMultipleTracks
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

let make = text => {
  switch (text) {
  | Some(text) =>
    switch (SpotifyUtils.isSpotifyCopy(text), Emoji.isEmoji(text)) {
    | (true, _) => SpotifyCopy(Utils.parseSpotifyCopy(text))
    | (_, true) => Emoji(Emoji.emojiCommand(text))
    | (_, _) =>
      switch (parseCommand(text)) {
      | "" => UnhandledCommand
      | "classics" => EasterEgg(IteamClassics)
      | "blame" => Blame
      | "clear" => Clear
      | "rm"
      | "remove" => RemoveMultipleTracks
      | "cq"
      | "currentqueue"
      | "gq"
      | "getqueue" => CurrentQueue
      | "freebird" => EasterEgg(FreeBird)
      | "friday" => EasterEgg(Friday)
      | "fq"
      | "fullqueue" => FullQueue
      | "help" => Help
      | "mute" => Mute
      | "mostplayed" => MostPlayed
      | "skip"
      | "next" => Next
      | "np"
      | "nowplaying" => NowPlaying
      | "pause" => Pause
      | "play" => Play
      | "playtrack" => PlayTrack
      | "previous" => Previous
      | "q"
      | "queue" => Queue
      | "cottoneyejoe"
      | "rednex" => EasterEgg(Rednex)
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
    }
  | None => UnhandledCommand
  };
};

let eggToString: EasterEgg.t => string =
  fun
  | IteamClassics => "classics"
  | FreeBird => "freebird"
  | Friday => "friday"
  | Rednex => "rednex"
  | Shoreline => "shoreline"
  | Slowdance => "slowdance"
  | Tequila => "tequila"
  | WWW => "world-wide-web";

let emojiToString: Emoji.t => string =
  fun
  | ThumbsUp => "volume-up"
  | ThumbsDown => "volume-down"
  | Santa => "santa"
  | _ => "unknown";

let commandToString =
  fun
  | Blame => "blame"
  | Clear => "clear"
  | CurrentQueue => "current-queue"
  | EasterEgg(egg) => "easteregg-" ++ eggToString(egg)
  | Emoji(emoji) => "emoji-" ++ emojiToString(emoji)
  | FullQueue => "full-queue"
  | Help => "help"
  | MostPlayed => "most-played"
  | Mute => "playback-mute"
  | Next => "playback-next"
  | NowPlaying => "now-playing"
  | Pause => "playback-pause"
  | Play => "playback-play"
  | PlayTrack => "playback-playtrack"
  | Previous => "playback-previous"
  | Queue => "queue"
  | RemoveMultipleTracks => "remove"
  | SpotifyCopy(_) => "spotify-copy"
  | Search => "search"
  | Time => "time"
  | Toplist => "toplist"
  | UnknownCommand(_) => "unknown-command"
  | UnhandledCommand => "unhandled-command"
  | Unmute => "playback-unmute"
  | Volume => "volume";
