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

let log = ({command, user, text}: Decode.event) => {
  switch (command) {
  | UnhandledCommand => ()
  | SpotifyCopy(copiedTracks) =>
    {"sender": user, "command": "spotify-copy", "args": copiedTracks}
    |> sendLog
  | _ =>
    {
      "sender": user,
      "command": Commands.commandToString(command),
      "args":
        switch (command) {
        | UnknownCommand(c) => [|c|]
        | _ =>
          Js.String.length(text) > 0 ? [|Utils.parsedTrack(text)|] : [||]
        },
    }
    |> sendLog
  };
};

let logNew = (command: Commands.t, user: string, text: string) => {
  switch (command) {
  | UnhandledCommand => ()
  | SpotifyCopy(copiedTracks) =>
    {"sender": user, "command": "spotify-copy", "args": copiedTracks}
    |> sendLog
  | _ =>
    {
      "sender": user,
      "command": Commands.commandToString(command),
      "args":
        switch (command) {
        | UnknownCommand(c) => [|c|]
        | _ =>
          Js.String.length(text) > 0 ? [|Utils.parsedTrack(text)|] : [||]
        },
    }
    |> sendLog
  };
};
