open Js.Promise;
open Tablecloth;

let message = (hits: Elastic.Search.t) =>
  switch (Array.length(hits)) {
  | 0 => Message.sorry
  | 1 =>
    switch (hits->Array.get(~index=0)) {
    | Some({sender}) =>
      Slack.User.make(sender) ++ " added this awesome track!"
    | None => Message.sorry
    }
  | _ =>
    "*This track has been added by*\n"
    ++ hits
       ->Array.mapWithIndex(~f=(i, hit) =>
           Utils.listNumber(i)
           ++ Slack.User.make(hit.sender)
           ++ " on "
           ++ Date.Format.date(hit.timestamp)
         )
       ->Utils.joinWithNewline
  };

module Request = {
  let make = uri => {
    let%Async response =
      API.createRequest(
        ~url=Config.blameUrl,
        ~_method="POST",
        ~data=Some({"uri": uri}),
        (),
      );

    response##data->Elastic.Search.make->resolve;
  };
};

let run = args =>
  switch (args) {
  | "" =>
    let%Async {uri} = Services.getCurrentTrack();
    let%Async response = Request.make(Utils.Sonos.toSpotifyUri(uri));

    Slack.Msg.make([`Section(message(response))]);
  | index =>
    let%Async queue = Queue.WithFallback.make();

    switch (queue) {
    | Queue({items}) =>
      switch (items->Array.get(~index=index->int_of_string - 1)) {
      | Some({uri}) =>
        let%Async response =
          Request.make(Utils.Sonos.toSpotifyUri(Some(uri)));

        Slack.Msg.make([`Section(message(response))]);
      | None =>
        Slack.Msg.make([`Section("Could not find track number " ++ index)])
      }
    | NoTracks => Slack.Msg.make([`Section(Message.emptyQueue)])
    };
  };
