let run = () => {
  let mostPlayed =
    {j|{
      "body": {
        "size": 0,
        "query":{
          "bool": {
            "should": [
              {
                "match": {
                  "command.keyword": "queue"
                }
              },
              {
                "match": {
                  "command.keyword": "spotify-copy"
                }
              }
            ]
          }
        },
        "aggs": {
          "most_played": {
            "terms": {
              "field": "args.keyword",
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
    Elastic.aggregate("most_played", mostPlayed)
    |> then_((hits: array(Elastic.Aggregate.t)) =>
         switch (Belt.Array.length(hits)) {
         | 0 => resolve(`Ok("No plays :sad_panda:"))
         | _ =>
           hits
           ->Belt.Array.map(({key}) => key->Utils.spotifyId)
           ->Belt.Array.map(id => Spotify.getTrack(id))
           |> all
           |> then_((tracks: array(Spotify.track)) => {
                let message =
                  "*Most played*\n"
                  ++ tracks
                     ->Belt.Array.mapWithIndex((i, {artists, name}) =>
                         Utils.listNumber(i)
                         ++ Spotify.buildArtist(artists)
                         ++ " - "
                         ++ name
                         ++ " ("
                         ++ string_of_int(hits[i].count)
                         ++ ")"
                       )
                     ->Utils.joinWithNewline;

                resolve(`Ok(message));
              })
         }
       )
  );
};
