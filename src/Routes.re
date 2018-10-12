open Express;

let handleVerification = body => {
  let verification = body |> Decode.verification;
  Response.sendString(verification##challenge);
};

let event =
  Middleware.from((_next, req, res) =>
    res
    |> (
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        switch ((body |> Decode.type_)##eventType) {
        | UrlVerification => handleVerification(body)
        | EventCallback =>
          Event.handleEventCallback(body);
          Response.sendStatus(Ok);
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
