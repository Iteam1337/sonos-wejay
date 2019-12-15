open Tablecloth;

let message = (hits: Elastic.Aggregate.t) =>
  switch (Array.length(hits)) {
  | 0 => Message.noPlays
  | _ =>
    "*Toplist*\n"
    ++ hits
       ->Array.mapWithIndex(~f=(i, {key: id, count}) =>
           Utils.listNumber(i)
           ++ Slack.User.make(id)
           ++ " ("
           ++ string_of_int(count)
           ++ ")"
         )
       ->Utils.joinWithNewline
  };

let run = () => {
  let%Async response = API.createRequest(~url=Config.toplistUrl, ());
  let resp = response##data->Elastic.Aggregate.make;

  Slack.Msg.make([`Section(message(resp))]);
};
