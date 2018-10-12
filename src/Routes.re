open Express;

let handleVerification = body => {
  let verification = body |> Decode.verification;
  Response.sendString(verification##challenge);
};

let handleEasterEgg = (egg: Decode.egg, sendMessage) =>
  switch (egg) {
  | IteamClassics =>
    sendMessage
    |> Services.queue("spotify:user:believer:playlist:445NQ4LkJFtBsHUOdr3LFI")
  | FreeBird =>
    sendMessage
    |> Services.queueEasterEgg("spotify:track:4qsAYBCJnu2OkTKUVbbOF1")
  | Friday =>
    switch (Js.Date.make() |> Js.Date.getDay) {
    | 5. =>
      sendMessage
      |> Services.queueEasterEgg("spotify:track:4fK6E2UywZTJIa5kWnCD6x")
    | _ => sendMessage("Sorry, it's not Friday")
    }
  | Shoreline =>
    sendMessage
    |> Services.queueEasterEgg("spotify:track:77jVczOFXfbdugN4djsIqs")
  | Slowdance =>
    sendMessage
    |> Services.queue("spotify:user:believer:playlist:5DQzhEf0U4Lji5kvXnPYSy")
  | Tequila =>
    sendMessage
    |> Services.queueEasterEgg("spotify:track:5gJKsGij5oGt5H5RSFYXPa")
  };

let help = sendMessage =>
  sendMessage(
    "*Queue and search*\n"
    ++ "```"
    ++ "currentqueue|getqueue _______________ Display upcoming tracks\n"
    ++ "nowplaying|np _______________________ Display what's playing right now\n"
    ++ "queue|q <Spotify URI> _______________ Queue a track using a Spotify URI\n"
    ++ "search|s <your search string here> __ Search Spotify for a track"
    ++ "```"
    ++ "\n\n*Play control*\n"
    ++ "```"
    ++ "clear _____________ Reset playlist\n"
    ++ "mute ______________ Mute\n"
    ++ "next ______________ Skip one track forward\n"
    ++ "pause _____________ Pause\n"
    ++ "play ______________ Play\n"
    ++ "previous __________ Go back one track\n"
    ++ "unmute ____________ Unmute\n"
    ++ "volume <0-100> ____ Set volume"
    ++ "```"
    ++ "\n\n*Misc*\n"
    ++ "```"
    ++ "help ______________ Display this message\n"
    ++ "```"
    ++ "\n\nThere's also some hidden easter eggs :hatching_chick:",
  );

let handleEventCallback = body => {
  let message = body |> Decode.message;
  let event = message##event;
  let sendMessage = Slack.sendSlackResponse(event##channel);
  let sendMessageWithAttachments =
    Slack.sendResponseWithAttachments(event##channel);
  let q = event##text;

  switch (event##subtype) {
  | Human =>
    switch (event##command) {
    | Search => sendMessageWithAttachments |> Spotify.searchTrack(q)
    | Clear => sendMessage |> Services.clearPlaylist
    | CurrentQueue => sendMessage |> Services.currentQueue
    | EasterEgg(egg) => sendMessage |> handleEasterEgg(egg)
    | Help => sendMessage |> help
    | Mute => Services.mute(true)
    | Next => Services.nextTrack()
    | NowPlaying => sendMessage |> Services.nowPlaying
    | Pause => sendMessage |> Services.pause
    | Play => Services.playTrack()
    | Previous => Services.previousTrack()
    | Queue => sendMessage |> Services.queue(q)
    | Unmute => Services.mute(false)
    | Volume => sendMessage |> Services.setVolume(q)
    | Unknown => ()
    }
  | Bot => ()
  };

  Response.sendStatus(Ok);
};

let event =
  Middleware.from((_next, req, res) =>
    res
    |> (
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        switch ((body |> Decode.type_)##eventType) {
        | UrlVerification => handleVerification(body)
        | EventCallback => handleEventCallback(body)
        | _ => Response.sendStatus(BadRequest)
        }
      | None => Response.sendStatus(BadRequest)
      }
    )
  );

let action =
  Middleware.from((_next, req, res) =>
    res
    |> (
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let response = body |> Decode.parseAction;
        let payload = response##payload |> Decode.actionPayload;

        Slack.sendSlackResponse(payload##channel##id)
        |> Services.queue(payload##actions[0]##value);

        Response.sendStatus(Ok);
      | None => Response.sendStatus(BadRequest)
      }
    )
  );
