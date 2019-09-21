let message = (hits: Elastic.Search.t) =>
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

module Request = {
  let make = uri => {
    Js.Promise.(
      API.createRequest(
        ~url=Config.blameUrl,
        ~_method="POST",
        ~data=Some({"uri": uri}),
        (),
      )
      |> then_(response => response##data->Elastic.Search.make->resolve)
    );
  };
};

let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(({uri}: Sonos.Decode.currentTrackResponse) => {
         let uri = Utils.sonosUriToSpotifyUri(uri);

         Request.make(uri)
         |> then_(response => `Ok(message(response))->resolve);
       })
  );
