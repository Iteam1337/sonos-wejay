open Express;

type t = {
  command: Commands.t,
  args: string,
};

let decode = json =>
  Json.Decode.{
    command: json |> field("command", string) |> Commands.decodeCommand,
    args: json |> field("args", string),
  };

let getToken = req => {
  let headers =
    switch (Js.Dict.get(Request.asJsonObject(req), "headers")) {
    | Some(h) => h
    | None => Js.Json.null
    };

  Json.Decode.(headers |> field("access_token", string));
};

let route =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let token = getToken(req);

        Slack.getUser(token)
        |> then_(user => {
             let {command, args} = body |> decode;

             let userId =
               switch (Js.Nullable.toOption(user)) {
               | None => None
               | Some(userId) => userId
               };

             Event.response(command, args, userId, Decode.Requester.Human)
             |> then_(response =>
                  switch (response) {
                  | `Ok(r) => res |> Response.sendString(r) |> resolve
                  | `Failed(_) =>
                    res |> Response.sendStatus(BadRequest) |> resolve
                  }
                );
           });
      | None => res |> Response.sendStatus(BadRequest) |> resolve
      }
    )
  );
