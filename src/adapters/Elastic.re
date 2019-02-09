let commandToString = (command: Commands.command) =>
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

let sendLog = data => {
  Js.Promise.(
    Axios.makeConfigWithUrl(
      ~url=Config.elasticUrl,
      ~_method="POST",
      ~data,
      (),
    )
    |> Axios.request
    |> then_(_ => resolve(true))
    |> catch(_ => resolve(false))
  )
  |> ignore;
};

let log = (event: Decode.event) => {
  switch (event.command) {
  | UnhandledCommand => ()
  | SpotifyCopy(copiedTracks) =>
    {
      "sender": event.user,
      "command": "spotify-copy",
      "args":
        copiedTracks->Belt.Array.map(t =>
          t
          |> Js.String.split(" ")
          |> Js.Array.sliceFrom(1)
          |> Js.Array.joinWith("")
        ),
    }
    |> sendLog
  | _ =>
    {
      "sender": event.user,
      "command": commandToString(event.command),
      "args":
        Js.String.length(event.text) > 0 ?
          [|event.text |> Js.String.replaceByRe([%re "/<|>/g"], "")|] : [||],
    }
    |> sendLog
  };
};
