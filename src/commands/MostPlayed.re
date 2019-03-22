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
    API.createRequest(~url=Config.mostPlayedUrl, ())
    |> then_(response => {
         let hits = Elastic.Aggregate.make(response##data);

         Belt.Array.(
           switch (length(hits)) {
           | 0 => resolve(`Ok(Messages.noPlays))
           | _ =>
             hits
             ->map(({key}) => key->SpotifyUtils.trackId)
             ->map(Spotify.getTrack)
             |> all
             |> then_(tracks => `Ok(message(tracks, hits)) |> resolve)
           }
         );
       })
  );
};
