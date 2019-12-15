open Tablecloth;

let message = (tracks: array(Spotify.Track.t), hits: Elastic.Aggregate.t) =>
  "*Most played*\n"
  ++ tracks
     ->Array.mapWithIndex(~f=(index, {artist, name}) => {
         let n = Utils.listNumber(index);

         switch (hits->Array.get(~index)) {
         | Some({count}) => {j|$n$artist - $name ($count)|j}
         | None => {j|$n$artist - $name|j}
         };
       })
     ->Utils.joinWithNewline;

let filterPlaylists = (doc: Elastic.Aggregate.aggregate) =>
  !Js.String.includes("playlist", doc.key);

let run = () => {
  let%Async response = API.createRequest(~url=Config.mostPlayedUrl, ());
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
      |> Js.Promise.all
      |> Js.Promise.then_(tracks =>
           Slack.Msg.make([`Section(message(tracks, hits))])
         )
    }
  );
};
