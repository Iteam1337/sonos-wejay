open Express;

let handleVerification = body => {
  let verification = body |> Decode.verification;
  Response.sendString(verification##challenge);
};

let handleEventCallback = body => {
  let message = body |> Decode.message;
  let event = message##event;
  let sendMessage = Slack.sendSlackResponse(event##channel);
  let sendMessageWithAttachments =
    Slack.sendResponseWithAttachments(event##channel);

  switch (event##command) {
  | Search => sendMessageWithAttachments |> Spotify.searchTrack(event##text)
  | Clear => sendMessage |> Services.clearPlaylist
  | CurrentQueue => sendMessage |> Services.currentQueue
  | CurrentTrack => sendMessage |> Services.currentTrack
  | Mute => Services.mute(true)
  | Next => Services.nextTrack()
  | Pause => sendMessage |> Services.pause
  | Play => Services.playTrack()
  | Previous => Services.previousTrack()
  | Queue => sendMessage |> Services.queueTrack(event##text)
  | Unmute => Services.mute(false)
  | Volume => sendMessage |> Services.setVolume(event##text)
  | Unknown => failwith("Unknown command")
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
        |> Services.queueTrack(payload##actions[0]##value);

        Response.sendStatus(Ok);
      | None => Response.sendStatus(BadRequest)
      }
    )
  );
