open Express;
open Js.Promise;

let badRequest = Response.sendStatus(BadRequest);

module IndexRoute = {
  let make =
    Middleware.from((_next, _req, res) =>
      res |> Response.sendString("Welcome to Wejay!")
    );
};

module EventRoute = {
  module VerificationCallback = {
    open Decode;

    let make = body => {
      let {challenge}: Verification.t = Verification.make(body);
      Response.sendString(challenge);
    };
  };

  module EventCallback = {
    let make = event => {
      let {channel, command, text: args, user}: Decode.Event.t = event;

      Event.make(~command, ~args, ~user, ())
      |> then_(response => {
           switch (response) {
           | Belt.Result.Ok(Slack.Result.Message(r)) =>
             Slack.Message.make(channel, r)
           | Belt.Result.Error(_) => ()
           };
           resolve();
         })
      |> ignore;

      Response.sendStatus(Ok);
    };
  };

  let make =
    PromiseMiddleware.from((_next, req, res) =>
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        res
        |> (
          switch (Decode.EventResponse.make(body)) {
          | UrlVerification => VerificationCallback.make(body)
          | Event(event) =>
            switch (event) {
            | Some(event) => EventCallback.make(event)
            | None => badRequest
            }
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
             | Belt.Result.Ok(Slack.Result.Message(m)) =>
               API.createRequest(
                 ~url=response_url,
                 ~_method="POST",
                 ~data=Some({"text": m[0]##text##text}),
                 (),
               )
               |> then_(_ => res |> Response.sendStatus(Ok) |> resolve)

             | Belt.Result.Error(_) => res |> badRequest |> resolve
             };
           });
      | None => res |> badRequest |> resolve
      }
    );
};
