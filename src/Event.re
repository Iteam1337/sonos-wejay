let handleEasterEgg = (egg: Commands.egg, user, sendMessage) =>
  Services.(
    switch (egg) {
    | IteamClassics =>
      queueAsLast(
        "spotify:user:believer:playlist:445NQ4LkJFtBsHUOdr3LFI",
        user,
        sendMessage,
      )
    | FreeBird =>
      queueAsNext("spotify:track:4qsAYBCJnu2OkTKUVbbOF1", user, sendMessage)
    | Friday =>
      switch (Js.Date.make() |> Js.Date.getDay) {
      | 5. =>
        queueAsNext("spotify:track:4fK6E2UywZTJIa5kWnCD6x", user, sendMessage)
      | _ => sendMessage("Sorry, it's not Friday") |> ignore
      }
    | Shoreline =>
      queueAsNext("spotify:track:77jVczOFXfbdugN4djsIqs", user, sendMessage)
    | Slowdance =>
      queueAsLast(
        "spotify:user:believer:playlist:5DQzhEf0U4Lji5kvXnPYSy",
        user,
        sendMessage,
      )
    | Tequila =>
      queueAsNext("spotify:track:5gJKsGij5oGt5H5RSFYXPa", user, sendMessage)
    }
  );

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
      | Blame => sendMessage |> blame
      | Clear => sendMessage |> clearPlaylist
      | CurrentQueue => sendMessage |> currentQueue
      | EasterEgg(egg) => sendMessage |> handleEasterEgg(egg, user)
      | Emoji(emoji) =>
        switch (emoji) {
        | ThumbsDown => sendMessage |> changeVolumeWithValue(-10.)
        | ThumbsUp => sendMessage |> changeVolumeWithValue(10.)
        }
      | Help => Utils.help |> sendMessage |> ignore
      | MostPlayed => sendMessage |> Database.mostPlayed
      | NowPlaying => sendMessage |> nowPlaying
      | PlayTrack => sendMessage |> playTrackNumber(q)
      | Queue => sendMessage |> queueAsLast(q, user)
      | Toplist => sendMessage |> Database.toplist
      | Volume =>
        switch (q) {
        | "" => sendMessage |> getCurrentVolume
        | _ => sendMessage |> setNewVolume(q)
        }

      /* Send message with attachments */
      | Library => sendMessageWithAttachments |> searchLibrary(q)
      | Search =>
        sendMessageWithAttachments |> Spotify.searchTrackWithMessage(q)

      /* Don't send a message */
      | Mute => mute(true)
      | Next => nextTrack()
      | Pause => pause()
      | Play => playTrack()
      | Previous => previousTrack()
      | Unmute => mute(false)
      | Unknown => ()
      }
    | Bot => ()
    }
  );
};
