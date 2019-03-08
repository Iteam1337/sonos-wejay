let run = () => {
  let toplist =
    {j|{
      "body": {
        "size": 0,
          "query": {
          "bool": {
            "must_not": [
              {
                "match": {
                  "sender.keyword": ""
                }
              }
            ]
          }
        },
        "aggs": {
          "toplist": {
            "terms": {
              "field": "sender.keyword",
              "order": {
                "_count": "desc"
              }
            }
          }
        }
      }
    }|j}
    |> Json.parseOrRaise;

  Js.Promise.(
    Elastic.aggregate("toplist", toplist)
    |> then_((hits: array(Elastic.Aggregate.t)) =>
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
         |> resolve
       )
  );
};
