let message = (hits: array(Elastic.Search.t)) =>
  switch (Belt.Array.length(hits)) {
  | 0 => "Sorry, I don't know who added this track"
  | 1 => Slack.userId(hits[0].sender) ++ " added this awesome track!"
  | _ =>
    "*This track has been added by*\n"
    ++ hits
       ->Belt.Array.mapWithIndex((i, hit) =>
           Utils.listNumber(i)
           ++ Slack.userId(hit.sender)
           ++ " on "
           ++ Utils.formatDate(hit.timestamp)
         )
       ->Utils.joinWithNewline
  };

let sonosUriToSpotifyUri = sonosUri => {
  let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

  Js.String.(
    switch (includes("x-file", sonosUri), sonosUri |> match(spotifyUri)) {
    | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
    | (true, None) => sonosUri
    | _ => ""
    }
  );
};

let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(({uri}: Sonos.Decode.currentTrackResponse) => {
         let uri = sonosUriToSpotifyUri(uri);

         API.createRequest(
           ~url=Config.blameUrl,
           ~_method="POST",
           ~data=Some({"uri": uri}),
           (),
         )
         |> then_(response =>
              `Ok(response##data->Elastic.Search.make->message)->resolve
            );
       })
  );
