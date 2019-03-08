open Express;

type t = {
  command: Commands.t,
  args: string,
  user: option(string),
};

let decode = json =>
  Json.Decode.{
    command: json |> field("command", string) |> Commands.decodeCommand,
    args: json |> field("args", string),
    user: json |> optional(field("user", string)),
  };

let route =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {command, args, user} = body |> decode;

        Event.response(command, args, user, Decode.Requester.Human)
        |> then_(response =>
             switch (response) {
             | `Ok(r) => res |> Response.sendString(r) |> resolve
             | `Failed(_) => res |> Response.sendStatus(BadRequest) |> resolve
             }
           );
      | None => res |> Response.sendStatus(BadRequest) |> resolve
      }
    )
  );
