let handleEasterEgg = (egg: Commands.egg, user, sendMessage) => {
  let qAsLast = uri => uri->Queue.asLast(user, sendMessage);
  let qAsNext = uri => uri->Queue.asNext(user, sendMessage);

  switch (egg) {
  | IteamClassics => SpotifyUtils.Playlists.iteamClassics->qAsLast
  | FreeBird => SpotifyUtils.Tracks.freeBird->qAsNext
  | Friday =>
    switch (Js.Date.make() |> Js.Date.getDay) {
    | 5. => SpotifyUtils.Tracks.friday->qAsNext
    | _ => sendMessage("Sorry, it's not Friday") |> ignore
    }
  | Shoreline => SpotifyUtils.Tracks.shoreline->qAsNext
  | Slowdance => SpotifyUtils.Playlists.slowdance->qAsLast
  | Tequila => SpotifyUtils.Tracks.tequila->qAsNext
  };
};

let handleEmoji = (emoji: Commands.emoji, sendMessage) =>
  switch (emoji) {
  | ThumbsDown => Volume.updateVolumeWithValue(-10., sendMessage)
  | ThumbsUp => Volume.updateVolumeWithValue(10., sendMessage)
  | Santa =>
    SpotifyUtils.Tracks.Christmas.getSong()->Queue.asNext(None, sendMessage)
  };

let handleEventCallback = body => {
  let {event}: Decode.message = body |> Decode.message;
  let {channel, text: q, subtype, command, user}: Decode.event = event;
  let sendMessage = Slack.sendSlackResponse(channel);
  let sendMessageWithAttachments = Slack.sendResponseWithAttachments(channel);

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
      | Help => sendMessage(Utils.help) |> ignore
      | MostPlayed => Database.mostPlayed(sendMessage)
      | NowPlaying => nowPlaying(sendMessage)
      | PlayTrack => Player.playTrackNumber(q, sendMessage)
      | Queue => Queue.asLast(q, user, sendMessage)
      | Time =>
        sendMessage(
          "This is Wejay!\nhttps://media.giphy.com/media/Ny4Ian52lZDz2/giphy.gif",
        )
        |> ignore
      | Toplist => Database.toplist(sendMessage)
      | Volume =>
        switch (q) {
        | "" => Volume.currentVolume(sendMessage)
        | _ => Volume.updateVolume(q, sendMessage)
        }

      /* Send message with attachments */
      | Library => Search.library(q, sendMessageWithAttachments)
      | Search =>
        Spotify.searchTrackWithMessage(q, sendMessageWithAttachments)

      /* Don't send a message */
      | Mute => Player.mute(true)
      | Next => Player.next()
      | Pause => Player.pause()
      | Play => Player.play()
      | Previous => Player.previous()
      | Unmute => Player.mute(false)
      | UnknownCommand(text) =>
        sendMessage(Utils.unknownCommand(text)) |> ignore
      }
    | Bot => ()
    }
  );
};