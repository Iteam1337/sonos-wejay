open Js.Promise;

module Log = {
  let make = (command, args, user) => {
    switch (user) {
    | Some(u) when u |> Js.String.length === 0 => ()
    | None => ()
    | Some(_) => Elastic.log(~command, ~text=args, ~user) |> ignore
    };
  };
};

let make = (~command, ~args, ~user, ()) => {
  Log.make(command, args, user);

  switch (command) {
  | Decode.Requester.Human(cmd) =>
    switch (cmd) {
    | NowPlaying => NowPlaying.run()
    | Search => Spotify.search(args)
    | SpotifyCopy(tracks) => Queue.multiple(tracks)

    /* Queue */
    | CurrentQueue => Queue.current()
    | FullQueue => Queue.full()
    | Clear => Queue.clear()
    | Queue => Queue.AsLastTrack.make(args, ())

    /* Player control */
    | Play => PlayerControl.play()
    | Pause => PlayerControl.pause()
    | Next => PlayerControl.next()
    | Previous => PlayerControl.previous()
    | PlayTrack => PlayerControl.playTrack(args)
    | Mute => PlayerControl.mute(true)
    | Unmute => PlayerControl.mute(false)
    | Volume => Volume.control(args)

    /* Misc */
    | Blame => Blame.run()
    | Emoji(emoji) => Emoji.make(emoji)
    | MostPlayed => MostPlayed.run()
    | Toplist => Toplist.run()
    | EasterEgg(egg) => EasterEgg.run(egg)
    | Help => `Ok(Slack.Block.make([`Section(Message.help)])) |> resolve
    | Time =>
      `Ok(Slack.Block.make([`Section(Message.thisIsWejay)])) |> resolve

    /* Unhandled and unknown */
    | UnknownCommand(cmd) =>
      `Ok(Slack.Block.make([`Section(Message.unknownCommand(cmd))]))
      |> resolve
    | UnhandledCommand => `Failed(Message.unhandledCommand) |> resolve
    }
  | Bot => resolve(`Failed(Message.botRequest))
  };
};
