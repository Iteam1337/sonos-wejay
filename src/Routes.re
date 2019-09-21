open Express;

let handleVerification = body => {
  let {challenge}: Decode.Verification.t = body |> Decode.Verification.make;
  Response.sendString(challenge);
};

let index =
  Middleware.from((_next, _req, res) =>
    res |> Response.sendString("Welcome to Wejay!")
  );

let message = request => {
  let {subtype, channel, command, text: args, user}: Decode.Event.t = request;
  let sendMessage = Slack.Message.regular(channel);

  Js.Promise.(
    Event.make(~command, ~args, ~user, ~subtype, ())
    |> then_(response => {
         switch (response) {
         | `Ok(r) => sendMessage(r)
         | `Failed(_) => ()
         };
         resolve();
       })
    |> ignore
  );
};

let messageWithBlocks = request => {
  let {subtype, channel, command, text: args, user}: Decode.Event.t = request;
  let sendSearchResponse = Slack.Message.withBlocks(channel);

  Js.Promise.(
    Event.makeWithBlocks(~command, ~args, ~user, ~subtype, ())
    |> then_(response => {
         switch (response) {
         | `Ok(message, blocks) => sendSearchResponse(message, blocks)
         | `Failed(_) => ()
         };

         resolve();
       })
    |> ignore
  );
};

let eventCallback = (event: option(Decode.Event.t), res) => {
  switch (event) {
  | Some(e) =>
    switch (e.command) {
    | Search
    | NowPlaying => messageWithBlocks(e)
    | _ => message(e)
    };

    res |> Response.sendStatus(Ok);
  | None => res |> Response.sendStatus(BadRequest)
  };
};

let failed = Response.sendStatus(BadRequest);

let event =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {eventType, event}: Decode.EventResponse.t =
          Decode.EventResponse.make(body);

        switch (eventType) {
        | UrlVerification => res |> handleVerification(body) |> resolve
        | EventCallback => res |> eventCallback(event) |> resolve
        | UnknownEvent => res |> failed |> resolve
        };
      | None => res |> failed |> resolve
      }
    )
  );

module Action = {
  open Js.Promise;

  let make =
    PromiseMiddleware.from((_next, req, res) =>
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {actions, response_url, user}: Decode.Action.t =
          body |> Decode.Action.make;
        let track = actions[0].value;

        Queue.last(track)
        |> then_(message => {
             Elastic.log(
               ~command=Commands.Queue,
               ~text=track,
               ~user=Some(user.id),
             );

             switch (message) {
             | `Ok(m) =>
               API.createRequest(
                 ~url=response_url,
                 ~_method="POST",
                 ~data=Some({"text": m}),
                 (),
               )
               |> then_(_ => res |> Response.sendString(m) |> resolve)

             | `Failed(_) => res |> failed |> resolve
             };
           });
      | None => res |> failed |> resolve
      }
    );
};

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
