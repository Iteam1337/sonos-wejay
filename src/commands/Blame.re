let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(({uri}: Sonos.Decode.currentTrackResponse) => {
         let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

         let uri =
           switch (
             Js.String.includes("x-file", uri),
             uri |> Js.String.match(spotifyUri),
           ) {
           | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
           | (true, None) => uri
           | _ => ""
           };

         let lastPlayedBy =
           {j|{
              "body": {
                "sort": [
                  {
                    "timestamp": {
                      "order": "desc"
                    }
                  }
                ],
                  "query": {
                "match": {
                  "args.keyword": "$uri"
                }
              }
              }
            }|j}
           |> Json.parseOrRaise;

         Elastic.query(lastPlayedBy)
         |> then_((hits: array(Elastic.Search.hit)) => {
              let message =
                switch (Belt.Array.length(hits)) {
                | 0 => "Sorry, I don't know who added this track"
                | 1 =>
                  Slack.userId(hits[0]._source.sender)
                  ++ " added this awesome track!"
                | _ =>
                  "*This track has been added by*\n"
                  ++ hits
                     ->Belt.Array.mapWithIndex((i, {_source}) =>
                         Utils.listNumber(i)
                         ++ Slack.userId(_source.sender)
                         ++ " on "
                         ++ Utils.formatDate(_source.timestamp)
                       )
                     ->Utils.joinWithNewline
                };

              resolve(`Ok(message));
            });
       })
  );
