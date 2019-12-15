let message = (tracks: array(Spotify.Track.t), hits: Elastic.Aggregate.t) =>
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
           | 0 => Slack.Msg.make([`Section(Message.noPlays)])
           | _ =>
             hits
             ->keep(filterPlaylists)
             ->map(({key}) => key->SpotifyUtils.trackId)
             ->keepMap(track => track)
             ->map(Spotify.Track.make)
             |> all
             |> then_(tracks =>
                  Slack.Msg.make([`Section(message(tracks, hits))])
                )
           }
         );
       })
    |> catch(_ => resolve(Error("Error in :: Most played")))
  );
};
