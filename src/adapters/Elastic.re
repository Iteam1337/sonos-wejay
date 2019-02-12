let sendLog = data => {
  Js.Promise.(
    Axios.makeConfigWithUrl(
      ~url=Config.elasticUrl,
      ~_method="POST",
      ~data,
      (),
    )
    |> Axios.request
    |> then_(_ => resolve(true))
    |> catch(_ => resolve(false))
  )
  |> ignore;
};

let log = (event: Decode.event) => {
  switch (event.command) {
  | UnhandledCommand => ()
  | SpotifyCopy(copiedTracks) =>
    {"sender": event.user, "command": "spotify-copy", "args": copiedTracks}
    |> sendLog
  | _ =>
    {
      "sender": event.user,
      "command": Commands.commandToString(event.command),
      "args":
        switch (event.command) {
        | UnknownCommand(c) => [|c|]
        | _ =>
          Js.String.length(event.text) > 0 ?
            [|event.text |> Js.String.replaceByRe([%re "/<|>/g"], "")|] :
            [||]
        },
    }
    |> sendLog
  };
};
