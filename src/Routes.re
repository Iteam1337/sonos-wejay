open Express;

let handleVerification = body => {
  let verification = body |> Decode.verification;
  Response.sendString(verification##challenge);
};

let handleEventCallback = body => {
  let message = body |> Decode.message;
  let event = message##event;
  let sendMessage = Slack.sendSlackResponse(event##channel);

  switch (event##command) {
  | Search =>
    Spotify.searchTrack(event##text) |> ignore;
    sendMessage("Searching for *" ++ event##text ++ "*", ());
  | Clear =>
    Services.clearPlaylist();
    sendMessage("Cleared queue", ());
  | CurrentQueue =>
    Services.currentQueue();
    sendMessage("Current queue", ());
  | Pause =>
    Services.pauseTrack();
    sendMessage("Paused", ());
  | Play => Services.playTrack()
  | Queue => sendMessage |> Services.queueTrack(event##text)
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
