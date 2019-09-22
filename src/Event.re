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

module Blocks = {
  let make = (~command, ~args, ~user, ()) => {
    Log.make(command, args, user);

    switch (command) {
    | Decode.Requester.Human(NowPlaying) => NowPlaying.run()
    | Human(Search) => Spotify.search(args)
    | Human(_) => resolve(`Failed(Message.unhandledCommand))
    | Bot => resolve(`Failed(Message.botRequest))
    };
  };
};

module Message = {
  let make = (~command, ~args, ~user, ()) => {
    Log.make(command, args, user);

    switch (command) {
    | Decode.Requester.Human(cmd) =>
      switch (cmd) {
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
      | Blame => Blame.run()
      | Emoji(emoji) => Emoji.handleEmoji(emoji)
      | MostPlayed => MostPlayed.run()
      | Toplist => Toplist.run()
      | EasterEgg(egg) => EasterEgg.run(egg)
      | Help => resolve(`Ok(Message.help))
      | Time => resolve(`Ok(Message.thisIsWejay))
      | UnknownCommand(command) =>
        resolve(`Ok(Message.unknownCommand(command)))

      /* Anything else */
      | UnhandledCommand => resolve(`Failed(Message.unhandledCommand))
      | NowPlaying
      | Search => resolve(`Failed("Handled as blocks"))
      }
    | Bot => resolve(`Failed(Message.botRequest))
    };
  };
};
