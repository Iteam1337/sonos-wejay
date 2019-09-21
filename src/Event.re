let logCommand = (command, args, user) => {
  switch (user) {
  | Some(u) when u |> Js.String.length === 0 => ()
  | None => ()
  | Some(_) => Elastic.log(~command, ~text=args, ~user) |> ignore
  };
};

let makeWithBlocks =
    (~command, ~args, ~user, ~subtype=Decode.Requester.Human, ()) => {
  logCommand(command, args, user);

  Js.Promise.(
    switch (subtype, command) {
    | (Human, NowPlaying) => NowPlaying.run()
    | (Human, Search) => Spotify.search(args)
    | (Human, _) =>
      resolve(`Failed("This is not the command you are looking for"))
    | (Bot, _) => resolve(`Failed("That's a bot request"))
    }
  );
};

let make = (~command, ~args, ~user, ~subtype=Decode.Requester.Human, ()) => {
  logCommand(command, args, user);

  Js.Promise.(
    switch (subtype) {
    | Human =>
      switch (command) {
      | Blame => Blame.run()

      /* Queue control */
      | Clear => Queue.clear()
      | CurrentQueue => Queue.current()
      | FullQueue => Queue.full()
      | Queue => Queue.last(args)
      | SpotifyCopy(tracks) => Queue.multiple(tracks)

      /* Player control */
      | Play => PlayerControl.play()
      | PlayTrack => PlayerControl.playTrack(args)
      | Pause => PlayerControl.pause()
      | Next => PlayerControl.next()
      | Previous => PlayerControl.previous()
      | Mute => PlayerControl.mute(true)
      | Unmute => PlayerControl.mute(false)
      | Volume => Volume.control(args)

      /* Misc */
      | Emoji(emoji) => Emoji.handleEmoji(emoji)
      | MostPlayed => MostPlayed.run()
      | Toplist => Toplist.run()
      | EasterEgg(egg) => EasterEgg.run(egg)
      | Help => resolve(`Ok(Messages.help))
      | Time => resolve(`Ok(Messages.thisIsWejay))
      | UnknownCommand(command) =>
        resolve(`Ok(Messages.unknownCommand(command)))

      /* Anything else */
      | _ => resolve(`Failed("This is not the command you are looking for"))
      }
    | Bot => resolve(`Failed("That's a bot request"))
    }
  );
};
