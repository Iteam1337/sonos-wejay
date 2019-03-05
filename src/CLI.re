open Express;

type t = {
  command: string,
  args: string,
  user: string,
};

let decode = json =>
  Json.Decode.{
    command: json |> field("command", string),
    args: json |> field("args", string),
    user: json |> field("user", string),
  };

let route =
  PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.(
      switch (Request.bodyJSON(req)) {
      | Some(body) =>
        let {command, args, user} = body |> decode;

        Event.response(command, args, user)
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
