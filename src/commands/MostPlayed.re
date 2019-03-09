let query =
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

let message =
    (tracks: array(Spotify.Track.t), hits: array(Elastic.Aggregate.t)) =>
  "*Most played*\n"
  ++ tracks
     ->Belt.Array.mapWithIndex((i, {artists, name}) => {
         let artist = Spotify.buildArtist(artists);
         let n = Utils.listNumber(i);
         let count = hits[i].count;

         {j|$n$artist - $name ($count)|j};
       })
     ->Utils.joinWithNewline;

let run = () => {
  Js.Promise.(
    Elastic.aggregate("most_played", query)
    |> then_((hits: array(Elastic.Aggregate.t)) =>
         Belt.Array.(
           switch (length(hits)) {
           | 0 => resolve(`Ok("No plays :sad_panda:"))
           | _ =>
             hits
             ->map(({key}) => key->SpotifyUtils.trackId)
             ->map(Spotify.getTrack)
             |> all
             |> then_(tracks => `Ok(message(tracks, hits)) |> resolve)
           }
         )
       )
  );
};
