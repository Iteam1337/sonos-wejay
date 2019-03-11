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
    Belt.Option.getWithDefault(
      Js.Dict.get(Request.asJsonObject(req), "headers"),
      Js.Json.null,
    );

  Json.Decode.(headers |> field("access_token", string));
};

let route =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let token = req |> getToken;

        token
        |> Slack.getUser
        |> then_(user => {
             let {command, args} = body |> decode;

             Event.response(
               command,
               args,
               Some(user),
               Decode.Requester.Human,
             )
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
