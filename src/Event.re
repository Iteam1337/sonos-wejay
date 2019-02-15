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
      | Blame => Misc.blame(sendMessage)
      | Clear => Queue.clearQueue(sendMessage)
      | CurrentQueue => Queue.currentQueue(sendMessage)
      | EasterEgg(egg) => EasterEgg.handleEasterEgg(egg, user, sendMessage)
      | Emoji(emoji) => Emoji.handleEmoji(emoji, sendMessage)
      | FullQueue => Queue.getFullQueue(sendMessage)
      | Help => sendMessage(Messages.help)
      | MostPlayed =>
        DbService.handleMostPlayed(sendMessage)->Database.mostPlayed
      | NowPlaying => nowPlaying(sendMessage)
      | Play => Player.play(sendMessage)
      | PlayTrack => Player.playTrackNumber(q, sendMessage)
      | SpotifyCopy(t) => Queue.addMultipleTracks(t, user, sendMessage)
      | Queue => Queue.asLast(~track=q, ~user, ~sendMessage, ()) |> ignore
      | Time => sendMessage(Messages.thisIsWejay)
      | Toplist => DbService.handleToplist(sendMessage)->Database.toplist
      | UnknownCommand(command) =>
        sendMessage(Messages.unknownCommand(command))
      | Volume => Volume.controlVolume(q, sendMessage)

      /* Send message with attachments */
      | Library => Search.library(q, sendMessageWithAttachments)
      | Search => Spotify.searchTrack(q, sendSearchResponse)

      /* Don't send a message */
      | Mute => Player.mute(true)
      | Next => Player.next()
      | Pause => Player.pause()
      | Previous => Player.previous()
      | Unmute => Player.mute(false)
      | UnhandledCommand => ()
      };

      /* Log all Human events to Elastic */
      Elastic.log(event);
    | Bot => ()
    }
  );
};
