let handleEventCallback = event => {
  let {channel, text: q, subtype, command, user}: Decode.event = event;
  let sendMessage = Slack.sendSlackResponse(channel);
  let sendMessageWithAttachments = Slack.sendResponseWithAttachments(channel);
  let sendSearchResponse = Slack.sendSearchResponse(channel);

  Services.(
    switch (subtype) {
    | Human =>
      switch (command) {
      /* Send string message */
      | Blame => Misc.blame(sendMessage) |> ignore
      | Clear => Queue.clearQueue(sendMessage)
      | CurrentQueue => Queue.currentQueue(sendMessage)
      | FullQueue => Queue.getFullQueue(sendMessage)
      | Help => sendMessage(Messages.help)
      | NowPlaying => nowPlaying(sendMessage)
      | Play => Player.play(sendMessage)
      | PlayTrack => Player.playTrackNumber(q, sendMessage)
      | Time => sendMessage(Messages.thisIsWejay)
      | Mute => Player.mute(true)
      | Next => Player.next()
      | Pause => Player.pause()
      | Previous => Player.previous()
      | Unmute => Player.mute(false)
      | UnknownCommand(command) =>
        sendMessage(Messages.unknownCommand(command))
      | EasterEgg(egg) => EasterEgg.handleEasterEgg(egg, user, sendMessage)
      | Queue => Queue.asLast(~track=q, ~user, ~sendMessage, ()) |> ignore
      | Volume => Volume.controlVolume(q, sendMessage)

      /* Unhandled below */
      | Emoji(emoji) => Emoji.handleEmoji(emoji, sendMessage)
      | SpotifyCopy(t) => Queue.addMultipleTracks(t, user, sendMessage)
      | Library => Search.library(q, sendMessageWithAttachments)
      | Search => Spotify.searchTrack(q, sendSearchResponse)

      /* DB */
      | MostPlayed =>
        DbService.handleMostPlayed(sendMessage)->Database.mostPlayed
      | Toplist => DbService.handleToplist(sendMessage)->Database.toplist

      /* Don't send a message */
      | UnhandledCommand => ()
      };

      /* Log all Human events to Elastic */
      Elastic.log(event);
    | Bot => ()
    }
  );
};

let response = (command, args, user) => {
  let cmd = command |> Commands.decodeCommand;

  Elastic.logNew(cmd, args, user) |> ignore;

  switch (cmd) {
  | Blame => Blame.run()
  | NowPlaying => NowPlaying.run()

  /* Queue control */
  | Clear => Queue.clear()
  | CurrentQueue => Queue.current()
  | FullQueue => Queue.full()
  | Queue => Queue.last(args)

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
  | EasterEgg(egg) => EasterEgg.run(egg)
  | Help => Js.Promise.resolve(`Ok(Messages.help))
  | Time => Js.Promise.resolve(`Ok(Messages.thisIsWejay))
  | UnknownCommand(command) =>
    Js.Promise.resolve(`Ok(Messages.unknownCommand(command)))

  /* Anything else */
  | _ =>
    Js.Promise.resolve(`Ok("This is not the command you are looking for"))
  };
};
