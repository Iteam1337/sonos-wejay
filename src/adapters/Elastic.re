module Client = {
  type t;
  type client;

  [@bs.deriving abstract]
  type config = {host: string};

  [@bs.new] [@bs.module "elasticsearch"]
  external esClient: config => client = "Client";
  [@bs.send]
  external search: (client, Js.Json.t) => Js.Promise.t(Js.Json.t) = "";

  let client = esClient(config(~host=Config.elasticUrl));
};

module Aggregate = {
  type t = {
    key: string,
    count: int,
  };

  let aggregations = json =>
    Json.Decode.{
      key: json |> field("key", string),
      count: json |> field("doc_count", int),
    };

  let aggregate = (aggregateValue, json) =>
    json
    |> Json.Decode.(
         at(
           ["aggregations", aggregateValue, "buckets"],
           array(aggregations),
         )
       );
};

module Search = {
  type t = {
    sender: string,
    timestamp: string,
  };
  type hit = {_source: t};

  let hitSource = json =>
    Json.Decode.{
      sender: json |> field("sender", string),
      timestamp: json |> field("timestamp", string),
    };

  let hit = json =>
    Json.Decode.{_source: json |> field("_source", hitSource)};

  let response = json =>
    json |> Json.Decode.(at(["hits", "hits"], array(hit)));
};

let query = searchQuery => {
  Js.Promise.(
    Client.client->Client.search(searchQuery)
    |> then_(response => response |> Search.response |> resolve)
  );
};

let aggregate = (aggregateValue, aggregateQuery) => {
  Js.Promise.(
    Client.client->Client.search(aggregateQuery)
    |> then_(response =>
         response |> Aggregate.aggregate(aggregateValue) |> resolve
       )
    |> catch(err => {
         Js.log(err);
         resolve([||]);
       })
  );
};

let sendLog = data => {
  Js.Promise.(
    Axios.makeConfigWithUrl(
      ~url=Config.elasticLogUrl,
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
