open Js.Promise;

let message = (hits: Elastic.Search.t) =>
  switch (Belt.Array.length(hits)) {
  | 0 => "Sorry, I don't know who added this track"
  | 1 => Slack.User.make(hits[0].sender) ++ " added this awesome track!"
  | _ =>
    "*This track has been added by*\n"
    ++ hits
       ->Belt.Array.mapWithIndex((i, hit) =>
           Utils.listNumber(i)
           ++ Slack.User.make(hit.sender)
           ++ " on "
           ++ DateFns.Format.date(hit.timestamp)
         )
       ->Utils.joinWithNewline
  };

module Request = {
  let make = uri => {
    API.createRequest(
      ~url=Config.blameUrl,
      ~_method="POST",
      ~data=Some({"uri": uri}),
      (),
    )
    |> then_(response => response##data->Elastic.Search.make->resolve);
  };
};

let run = args =>
  switch (args) {
  | "" =>
    Services.getCurrentTrack()
    |> then_(({uri}: Sonos.Decode.CurrentTrack.t) =>
         Request.make(Utils.Sonos.toSpotifyUri(uri))
       )
    |> then_(response => Slack.Msg.make([`Section(message(response))]))
  | index =>
    Queue.queueWithFallback()
    |> then_(({items}: Sonos.Decode.CurrentQueue.t) =>
         switch (items->Belt.Array.get(index->int_of_string - 1)) {
         | Some({uri}) =>
           Request.make(Utils.Sonos.toSpotifyUri(Some(uri)))
           |> then_(response =>
                Slack.Msg.make([`Section(message(response))])
              )
         | None =>
           Slack.Msg.make([
             `Section("Could not find track number " ++ index),
           ])
         }
       )
  };
