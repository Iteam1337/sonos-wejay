let run = () => {
  Js.Promise.(
    API.createRequest(~url=Config.toplistUrl, ())
    |> then_(response => {
         let hits = Elastic.Aggregate.make(response##data);
         (
           switch (Belt.Array.length(hits)) {
           | 0 => `Ok("No plays :sad_panda:")
           | _ =>
             let message =
               "*Toplist*\n"
               ++ hits
                  ->Belt.Array.mapWithIndex((i, {key, count}) =>
                      Utils.listNumber(i)
                      ++ Slack.userId(key)
                      ++ " ("
                      ++ string_of_int(count)
                      ++ ")"
                    )
                  ->Utils.joinWithNewline;

             `Ok(message);
           }
         )
         |> resolve;
       })
  );
};
