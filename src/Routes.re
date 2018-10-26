open Express;

let handleVerification = body => {
  let {challenge}: Decode.verification = body |> Decode.verification;
  Response.sendString(challenge);
};

let event =
  Middleware.from((_next, req, res) =>
    res
    |> (
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {eventType}: Decode.messageType = Decode.type_(body);

        switch (eventType) {
        | UrlVerification => handleVerification(body)
        | EventCallback =>
          Event.handleEventCallback(body);
          Response.sendStatus(Ok);
        | _ => Response.sendStatus(BadRequest)
        };
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

        Database.insertTrack(
          ~uri=payload.actions[0].value,
          ~user=payload.user.id,
          ~time=Js.Math.abs_int(Js.Date.now() |> int_of_float),
        );

        Slack.sendSlackResponse(payload.channel.id)
        |> Services.queueAsLast(payload.actions[0].value);

        Response.sendStatus(Ok);
      | None => Response.sendStatus(BadRequest)
      }
    )
  );
