let handleEasterEgg = (egg: Commands.egg, user, sendMessage) => {
  let qAsLast = track =>
    Queue.asLast(~track, ~user, ~sendMessage, ()) |> ignore;
  let qAsNext = uri => uri->Queue.asNext(user, sendMessage);

  SpotifyUtils.(
    switch (egg) {
    | IteamClassics => Playlists.iteamClassics->qAsLast
    | FreeBird => Tracks.freeBird->qAsNext
    | Friday =>
      switch (Js.Date.make() |> Js.Date.getDay) {
      | 5. => Tracks.friday->qAsNext
      | _ => sendMessage("Sorry, it's not Friday") |> ignore
      }
    | Shoreline => Tracks.shoreline->qAsNext
    | Slowdance => Playlists.slowdance->qAsLast
    | Tequila => Tracks.tequila->qAsNext
    | WWW => Tracks.worldwideweb->qAsNext
    }
  );
};

let handleEmoji = (emoji: Commands.emoji, sendMessage) =>
  switch (emoji) {
  | ThumbsDown => Volume.updateVolumeWithValue(-10., sendMessage)
  | ThumbsUp => Volume.updateVolumeWithValue(10., sendMessage)
  | Santa =>
    SpotifyUtils.Tracks.Christmas.getSong()->Queue.asNext(None, sendMessage)
  };

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
      | EasterEgg(egg) => handleEasterEgg(egg, user, sendMessage)
      | Emoji(emoji) => handleEmoji(emoji, sendMessage)
      | FullQueue => Queue.getFullQueue(sendMessage)
      | Help => Utils.help->sendMessage
      | MostPlayed => Database.mostPlayed(sendMessage)
      | NowPlaying => nowPlaying(sendMessage)
      | Play => Player.play(sendMessage)
      | PlayTrack => Player.playTrackNumber(q, sendMessage)
      | SpotifyCopy(t) => Queue.addMultipleTracks(t, user, sendMessage)
      | Queue => Queue.asLast(~track=q, ~user, ~sendMessage, ()) |> ignore
      | Time => Utils.thisIsWejay->sendMessage
      | Toplist => Database.toplist(sendMessage)
      | UnknownCommand(text) => Utils.unknownCommand(text)->sendMessage
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
      }
    | Bot => ()
    }
  );
};
