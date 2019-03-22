module Aggregate = {
  type t = {
    key: string,
    count: int,
  };

  let aggregation = json =>
    Json.Decode.{
      key: json |> field("key", string),
      count: json |> field("doc_count", int),
    };

  let make = json => json |> Json.Decode.array(aggregation);
};

module Search = {
  type t = {
    sender: string,
    timestamp: string,
  };

  let hit = json =>
    Json.Decode.{
      sender: json |> field("sender", string),
      timestamp: json |> field("timestamp", string),
    };

  let make = json => json |> Json.Decode.array(hit);
};

let sendLog = data => {
  Js.Promise.(
    API.createRequest(
      ~url=Config.elasticLogUrl,
      ~_method="POST",
      ~data=Some(data),
      (),
    )
    |> then_(_ => resolve(true))
    |> catch(_ => resolve(false))
  )
  |> ignore;
};

let handleLog = (command: Commands.t, user, text) => {
  switch (user) {
  | Some(sender) =>
    switch (command) {
    | UnhandledCommand => ()
    | SpotifyCopy(copiedTracks) =>
      {"sender": sender, "command": "spotify-copy", "args": copiedTracks}
      |> sendLog
    | _ =>
      {
        "sender": sender,
        "command": Commands.commandToString(command),
        "args":
          switch (command) {
          | UnknownCommand(c) => [|c|]
          | _ =>
            Js.String.length(text) > 0 ? [|Utils.parsedTrack(text)|] : [||]
          },
      }
      |> sendLog
    }
  | None => ()
  };
};

let log = ({command, user, text}: Decode.event) => {
  handleLog(command, user, text);
};

let logNew = (command: Commands.t, text: string, user: option(string)) => {
  handleLog(command, user, text);
};
