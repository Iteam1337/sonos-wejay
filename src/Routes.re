open Express;

let handleVerification = body => {
  let {challenge}: Decode.verification = body |> Decode.verification;
  Response.sendString(challenge);
};

let index =
  Middleware.from((_next, _req, res) =>
    res |> Response.sendString("Welcome to Wejay!")
  );

let globalEvent = ({subtype, channel, command, text, user}: Decode.event) => {
  let sendMessage = Slack.sendSlackResponse(channel);

  Js.Promise.(
    Event.response(command, text, user, subtype)
    |> then_(response => {
         switch (response) {
         | `Ok(r) => sendMessage(r)
         | `Failed(e) => sendMessage({j|:warning: $e|j})
         };
         resolve();
       })
    |> ignore
  );
};

let eventCallback = (event: Decode.event) => {
  switch (event.command) {
  | Library
  | Search => Event.handleEventCallback(event)
  | _ => globalEvent(event)
  };
};

let failed = Response.sendStatus(BadRequest);

let event =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {eventType, event}: Decode.eventPayload =
          Decode.eventPayload(body);

        switch (eventType) {
        | UrlVerification => res |> handleVerification(body) |> resolve
        | EventCallback =>
          switch (event) {
          | Some(e) =>
            eventCallback(e);
            res |> Response.sendStatus(Ok) |> resolve;
          | None => res |> failed |> resolve
          }
        | UnknownEvent => res |> failed |> resolve
        };
      | None => res |> failed |> resolve
      }
    )
  );

let action =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let response = body->Decode.parseAction;
        let {actions, user}: Decode.actionPayload =
          response##payload->Decode.actionPayload;
        let track = actions[0].value;

        Queue.last(track)
        |> then_(message => {
             Elastic.logNew(Commands.Queue, track, Some(user.id));

             switch (message) {
             | `Ok(m) => res |> Response.sendString(m) |> resolve
             | `Failed(_) => res |> failed |> resolve
             };
           });
      | None => res |> failed |> resolve
      }
    )
  );

let slackAuth =
  Middleware.from((_next, _req, res) =>
    res
    |> Response.redirectCode(
         301,
         "https://slack.com/oauth/authorize?scope=identity.basic&client_id="
         ++ Config.slackClientId,
       )
  );

let slackToken =
  PromiseMiddleware.from((_next, req, res) => {
    let query = Request.query(req);

    let code =
      switch (Js.Dict.get(query, "code")) {
      | Some(json) => Js.Json.decodeString(json)
      | _ => None
      };

    switch (code) {
    | Some(c) =>
      Js.Promise.(
        Slack.makeAuthCallback(c)
        |> then_(response =>
             resolve(
               Response.sendString(
                 Belt.Option.getWithDefault(response##data##access_token, ""),
                 res,
               ),
             )
           )
      )
    | None => Js.Promise.resolve(Response.sendStatus(BadRequest, res))
    };
  });
