open Express;

[@decco]
type cli = {
  command: string,
  args: string,
};

type t = {
  command: Commands.t,
  args: string,
};

let decode: Js.Json.t => t =
  json =>
    switch (cli_decode(json)) {
    | Ok(output) => {
        args: output.args,
        command: Commands.make(Some(output.command)),
      }
    | Error({Decco.path, message}) => Parser.fail(message, path)
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
             let {command, args} = decode(body);

             Event.make(~command, ~args, ~user=Some(user), ())
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
