open Express;

let handleVerification = body => {
  let {challenge}: Decode.verification = body |> Decode.verification;
  Response.sendString(challenge);
};

let index =
  Middleware.from((_next, _req, res) =>
    res |> Response.sendString("Welcome to Wejay!")
  );

let event =
  Middleware.from((_next, req, res) =>
    res
    |> (
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {eventType, event}: Decode.eventPayload =
          Decode.eventPayload(body);

        switch (eventType) {
        | UrlVerification => handleVerification(body)
        | EventCallback =>
          switch (event) {
          | Some(e) =>
            Event.handleEventCallback(e);
            Response.sendStatus(Ok);
          | None => Response.sendStatus(BadRequest)
          }

        | _ => Response.sendStatus(BadRequest)
        };
      | None => Response.sendStatus(BadRequest)
      }
    )
  );

let action =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let response = body->Decode.parseAction;
        let {actions, user, channel}: Decode.actionPayload =
          response##payload->Decode.actionPayload;

        Queue.asLast(~track=actions[0].value, ~user=Some(user.id), ())
        |> then_(message => {
             Elastic.log({
               channel: channel.id,
               command: Commands.Queue,
               user: Some(user.id),
               text: actions[0].value,
               subtype: Decode.Requester.Human,
             });

             res |> Response.sendString(message) |> resolve;
           });
      | None => res |> Response.sendStatus(BadRequest) |> resolve
      }
    )
  );
