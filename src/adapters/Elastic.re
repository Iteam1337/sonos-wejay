[@bs.obj]
external elasticLog:
  (~sender: string, ~command: string, ~args: array(string)) => _ =
  "";

module Aggregate = {
  [@decco]
  type aggregate = {
    key: string,
    [@decco.key "doc_count"]
    count: int,
  };

  [@decco]
  type t = array(aggregate);

  let make = json => t_decode(json)->Parser.handle;
};

module Search = {
  [@decco]
  type search = {
    sender: string,
    timestamp: string,
  };

  [@decco]
  type t = array(search);

  let make = json => t_decode(json)->Parser.handle;
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
