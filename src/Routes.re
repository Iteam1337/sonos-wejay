open Express;
open Js.Promise;

let badRequest = Response.sendStatus(BadRequest);

module IndexRoute = {
  let make =
    Middleware.from((_next, _req, res) =>
      res |> Response.sendString("Welcome to Wejay!")
    );
};

module VerificationCallback = {
  open Decode;

  let make = body => {
    let {challenge}: Verification.t = Verification.make(body);
    Response.sendString(challenge);
  };
};

module CreateMessage = {
  let make = (~request, ~messenger, ~creator) => {
    let {channel, command, text: args, user}: Decode.Event.t = request;

    creator(~command, ~args, ~user, ())
    |> then_(response => {
         switch (response) {
         | `Ok(r) => messenger(channel, r)
         | `Failed(_) => ()
         };
         resolve();
       })
    |> ignore;
  };

  let message = request => {
    make(
      ~request,
      ~messenger=Slack.Message.Regular.make,
      ~creator=Event.Message.make,
    );
  };

  let blocks = request => {
    make(
      ~request,
      ~messenger=Slack.Message.Blocks.make,
      ~creator=Event.Blocks.make,
    );
  };
};

module EventCallback = {
  let make = (event: option(Decode.Event.t), res) => {
    switch (event) {
    | Some({command} as e) =>
      switch (command) {
      | Human(Search)
      | Human(NowPlaying) => CreateMessage.blocks(e)
      | _ => CreateMessage.message(e)
      };

      res |> Response.sendStatus(Ok);
    | None => res |> Response.sendStatus(BadRequest)
    };
  };
};

module EventRoute = {
  let make =
    PromiseMiddleware.from((_next, req, res) =>
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        res
        |> (
          switch (Decode.EventResponse.make(body)) {
          | UrlVerification => VerificationCallback.make(body)
          | EventCallback(event) => EventCallback.make(event)
          | UnknownEvent => badRequest
          }
        )
        |> resolve
      | None => res |> badRequest |> resolve
      }
    );
};

module ActionRoute = {
  let make =
    PromiseMiddleware.from((_next, req, res) =>
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {actions, response_url, user}: Decode.Action.t =
          body |> Decode.Action.make;
        let track = actions[0].value;

        Queue.AsLastTrack.make(track, ())
        |> then_(message => {
             Elastic.log(
               ~command=Human(Commands.Queue),
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

             | `Failed(_) => res |> badRequest |> resolve
             };
           });
      | None => res |> badRequest |> resolve
      }
    );
};

module SlackRoutes = {
  let auth =
    Middleware.from((_next, _req, res) =>
      res
      |> Response.redirectCode(
           301,
           "https://slack.com/oauth/authorize?scope=identity.basic&client_id="
           ++ Config.slackClientId,
         )
    );

  let token =
    PromiseMiddleware.from((_next, req, res) => {
      let query = Request.query(req);

      let code =
        switch (Js.Dict.get(query, "code")) {
        | Some(json) => Js.Json.decodeString(json)
        | _ => None
        };

      switch (code) {
      | Some(c) =>
        Slack.makeAuthCallback(c)
        |> then_(response =>
             resolve(
               Response.sendString(
                 Belt.Option.getWithDefault(response##data##access_token, ""),
                 res,
               ),
             )
           )
      | None => res |> badRequest |> resolve
      };
    });
};
