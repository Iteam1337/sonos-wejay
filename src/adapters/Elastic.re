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
  let%Async _ =
    API.createRequest(
      ~url=Config.elasticLogUrl,
      ~_method="POST",
      ~data=Some(data),
      (),
    );

  Js.Promise.resolve();
};

let log = (~command: Decode.Requester.t, ~text, ~user) => {
  switch (user, command) {
  | (u, Human(cmd)) =>
    switch (u, cmd) {
    | (Some(_), UnhandledCommand)
    | (None, _) => ()
    | (Some(sender), SpotifyCopy(copiedTracks)) =>
      elasticLog(~sender, ~command="spotify-copy", ~args=copiedTracks)
      ->sendLog
      ->ignore
    | (Some(sender), _) =>
      elasticLog(
        ~sender,
        ~command=Commands.toString(cmd),
        ~args=
          switch (cmd) {
          | UnknownCommand(c) => [|c|]
          | _ =>
            Js.String.length(text) > 0 ? [|Utils.parsedTrack(text)|] : [||]
          },
      )
      ->sendLog
      ->ignore
    }
  | (_, Bot) => ()
  };
};
