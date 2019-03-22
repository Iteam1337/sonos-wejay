let message = (hits: array(Elastic.Aggregate.t)) =>
  switch (Belt.Array.length(hits)) {
  | 0 => `Ok(Messages.noPlays)
  | _ =>
    `Ok(
      "*Toplist*\n"
      ++ hits
         ->Belt.Array.mapWithIndex((i, {key, count}) =>
             Utils.listNumber(i)
             ++ Slack.userId(key)
             ++ " ("
             ++ string_of_int(count)
             ++ ")"
           )
         ->Utils.joinWithNewline,
    )
  };

let run = () => {
  Js.Promise.(
    API.createRequest(~url=Config.toplistUrl, ())
    |> then_(response =>
         response##data->Elastic.Aggregate.make->message->resolve
       )
  );
};
