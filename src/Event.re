let handleEasterEgg = (egg: Commands.egg, sendMessage) =>
  switch (egg) {
  | IteamClassics =>
    sendMessage
    |> Services.queueAsLast(
         "spotify:user:believer:playlist:445NQ4LkJFtBsHUOdr3LFI",
       )
  | FreeBird =>
    sendMessage
    |> Services.queueEasterEgg("spotify:track:4qsAYBCJnu2OkTKUVbbOF1")
  | Friday =>
    switch (Js.Date.make() |> Js.Date.getDay) {
    | 5. =>
      sendMessage
      |> Services.queueEasterEgg("spotify:track:4fK6E2UywZTJIa5kWnCD6x")
    | _ => sendMessage("Sorry, it's not Friday") |> ignore
    }
  | Shoreline =>
    sendMessage
    |> Services.queueEasterEgg("spotify:track:77jVczOFXfbdugN4djsIqs")
  | Slowdance =>
    sendMessage
    |> Services.queueAsLast(
         "spotify:user:believer:playlist:5DQzhEf0U4Lji5kvXnPYSy",
       )
  | Tequila =>
    sendMessage
    |> Services.queueEasterEgg("spotify:track:5gJKsGij5oGt5H5RSFYXPa")
  };

let handleEventCallback = body => {
  let {event}: Decode.message = body |> Decode.message;
  let {channel, text: q, subtype, command}: Decode.event = event;
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
      | EasterEgg(egg) => sendMessage |> handleEasterEgg(egg)
      | Help => Utils.help |> sendMessage |> ignore
      | NowPlaying => sendMessage |> nowPlaying
      | PlayTrack => sendMessage |> playTrackNumber(q)
      | Queue => sendMessage |> queueAsLast(q)
      | Volume =>
        switch (q) {
        | "" => sendMessage |> getVolume
        | _ => sendMessage |> setVolume(q)
        }

      /* Send message with attachments */
      | Library => sendMessageWithAttachments |> searchLibrary(q)
      | Search => sendMessageWithAttachments |> Spotify.searchTrack(q)

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
