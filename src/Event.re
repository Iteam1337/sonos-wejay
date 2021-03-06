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

  switch (command, args) {
  | (Decode.Requester.Human(cmd), "help") => Help.make(Some(cmd))
  | (Decode.Requester.Human(cmd), _) =>
    switch (cmd) {
    | NowPlaying => NowPlaying.run()
    | Search => Spotify.Search.make(args)
    | SpotifyCopy(tracks) => Queue.multiple(tracks)

    /* Queue */
    | CurrentQueue => Queue.current()
    | FullQueue => Queue.full()
    | Clear => Queue.clear()
    | Queue => Queue.AsLastTrack.make(args, ())
    | RemoveMultipleTracks => Queue.removeMultipleTracks(args)

    /* Player control */
    | Play => PlayerControl.play()
    | Pause => PlayerControl.pause()
    | Next => PlayerControl.next()
    | Previous => PlayerControl.previous()
    | PlayTrack => PlayerControl.playTrack(args)
    | Mute => PlayerControl.mute(true)
    | Unmute => PlayerControl.mute(false)
    | Volume => Volume.control(args)
    | PlayLatestTrack => PlayerControl.playLatestTrack()

    /* Misc */
    | Blame => Blame.run(args)
    | Emoji(emoji) => Emoji.make(emoji)
    | MostPlayed => MostPlayed.run()
    | Toplist => Toplist.run()
    | EasterEgg(egg) => EasterEgg.run(egg)
    | Help => Help.make(None)
    | Time => Slack.Msg.make([`Section(Message.thisIsWejay)])

    /* Unhandled and unknown */
    | UnknownCommand(cmd) =>
      Slack.Msg.make([`Section(Message.unknownCommand(cmd))])
    | UnhandledCommand => Error(Message.unhandledCommand) |> resolve
    }
  | (Bot, _) => resolve(Error(Message.botRequest))
  };
};
