let message =
    (tracks: array(Spotify.WejayTrack.t), hits: Elastic.Aggregate.t) =>
  "*Most played*\n"
  ++ tracks
     ->Belt.Array.mapWithIndex((i, {artist, name}) => {
         let n = Utils.listNumber(i);
         let count = hits[i].count;

         {j|$n$artist - $name ($count)|j};
       })
     ->Utils.joinWithNewline;

let filterPlaylists = (doc: Elastic.Aggregate.aggregate) =>
  !Js.String.includes("playlist", doc.key);

let run = () => {
  Js.Promise.(
    API.createRequest(~url=Config.mostPlayedUrl, ())
    |> then_(response => {
         let hits = Elastic.Aggregate.make(response##data);

         Belt.Array.(
           switch (length(hits)) {
           | 0 =>
             resolve(`Ok(Slack.Block.Simple.make(~message=Message.noPlays)))
           | _ =>
             hits
             ->keep(filterPlaylists)
             ->map(({key}) => key->SpotifyUtils.trackId)
             ->keepMap(track => track)
             ->map(Spotify.getSpotifyTrack)
             |> all
             |> then_(tracks =>
                  `Ok(
                    Slack.Block.Simple.make(~message=message(tracks, hits)),
                  )
                  |> resolve
                )
           }
         );
       })
    |> catch(err => {
         Js.log(err);
         resolve(`Failed("Error in :: Most played"));
       })
  );
};
