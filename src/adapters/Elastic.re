[@bs.obj]
external elasticLog:
  (~sender: string, ~command: string, ~args: array(string)) => _ =
  "";

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

let log = (~command: Commands.t, ~text, ~user) => {
  switch (user, command) {
  | (Some(_), UnhandledCommand)
  | (None, _) => ()
  | (Some(sender), SpotifyCopy(copiedTracks)) =>
    elasticLog(~sender, ~command="spotify-copy", ~args=copiedTracks)
    |> sendLog
  | (Some(sender), _) =>
    elasticLog(
      ~sender,
      ~command=Commands.commandToString(command),
      ~args=
        switch (command) {
        | UnknownCommand(c) => [|c|]
        | _ =>
          Js.String.length(text) > 0 ? [|Utils.parsedTrack(text)|] : [||]
        },
    )
    |> sendLog
  };
};
