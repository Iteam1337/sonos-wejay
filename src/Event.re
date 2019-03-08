/* Deprecated usage of callbacks, these aren't handle in the new way yet */
let handleEventCallback = event => {
  let {channel, text: q, subtype, command}: Decode.event = event;
  let sendMessageWithAttachments = Slack.sendResponseWithAttachments(channel);
  let sendSearchResponse = Slack.sendSearchResponse(channel);

  switch (subtype) {
  | Human =>
    switch (command) {
    /* Unhandled below */
    | Library => Search.library(q, sendMessageWithAttachments)
    | Search => Spotify.searchTrack(q, sendSearchResponse)
    | _ => ()
    };

    /* Log all Human events to Elastic */
    Elastic.log(event);
  | Bot => ()
  };
};

let response = (command, args, user, subtype: Decode.Requester.t) => {
  Elastic.logNew(command, args, user) |> ignore;

  switch (subtype) {
  | Human =>
    switch (command) {
    | Blame => Blame.run()
    | NowPlaying => NowPlaying.run()

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
    | Help => Js.Promise.resolve(`Ok(Messages.help))
    | Time => Js.Promise.resolve(`Ok(Messages.thisIsWejay))
    | UnknownCommand(command) =>
      Js.Promise.resolve(`Ok(Messages.unknownCommand(command)))

    /* Anything else */
    | _ =>
      Js.Promise.resolve(
        `Failed("This is not the command you are looking for"),
      )
    }
  | Bot => Js.Promise.resolve(`Failed("That's a bot request"))
  };
};
