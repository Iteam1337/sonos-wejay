open Tablecloth;

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
  | PlayLatestTrack
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
  |> String.toLower
  |> String.split(~on=" ")
  |> List.getAt(~index=0)
  |> Option.withDefault(~default="");

module CommandType = {
  type t =
    | SpotifyCommand(array(string))
    | EmojiCommand(Emoji.t)
    | Command(string);

  let make = input => {
    switch (SpotifyUtils.isSpotifyCopy(input), Emoji.isEmoji(input)) {
    | (true, _) => SpotifyCommand(Utils.parseSpotifyCopy(input))
    | (_, true) => EmojiCommand(Emoji.emojiCommand(input))
    | _ => Command(parseCommand(input))
    };
  };
};

let make = text => {
  switch (text) {
  | Some(text) =>
    switch (CommandType.make(text)) {
    | SpotifyCommand(tracks) => SpotifyCopy(tracks)
    | EmojiCommand(emoji) => Emoji(emoji)
    | Command(cmd) =>
      switch (cmd) {
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
      | "pl"
      | "playlatest" => PlayLatestTrack
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

let toString =
  fun
  | Blame => "blame"
  | Clear => "clear"
  | CurrentQueue => "current-queue"
  | EasterEgg(egg) => "easteregg-" ++ EasterEgg.toString(egg)
  | Emoji(emoji) => "emoji-" ++ Emoji.toString(emoji)
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
  | PlayLatestTrack => "play-latest-track"
  | Volume => "volume";
