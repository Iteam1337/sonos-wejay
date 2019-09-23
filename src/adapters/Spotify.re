[@bs.module "@wejay/spotify"]
external _getTrack: string => Js.Promise.t(Js.Json.t) = "getTrack";

[@bs.module "@wejay/spotify"]
external _search: string => Js.Promise.t(Js.Json.t) = "search";

module WejayTrack = {
  [@decco]
  type t = {
    albumName: string,
    artist: string,
    cover: string,
    duration: float,
    id: string,
    name: string,
    releaseDate: string,
    uri: string,
  };

  [@decco]
  type tracks = array(t);

  let tracks = json => tracks_decode(json)->Parser.handle;
  let track = json => t_decode(json)->Parser.handle;
};

let getSpotifyTrack = id => {
  Js.Promise.(
    _getTrack(id)
    |> then_(response => response |> WejayTrack.track |> resolve)
  );
};

let createSearchAttachment =
    ({albumName, artist, cover, duration, name, uri}: WejayTrack.t) => {
  let trackDuration = Utils.parseDuration(duration /. 1000.0);

  Slack.Block.make([
    `Divider,
    `FieldsWithImage({
      accessory: `Image((cover, "Album cover")),
      fields: [
        `Text({j|*Artist*\n$artist|j}),
        `Text({j|*Track name*\n$name|j}),
        `Text({j|*Album*\n$albumName|j}),
        `Text({j|*Current position*\n$trackDuration|j}),
      ],
    }),
    `Actions([
      `Button({
        text: "Queue track",
        value: uri,
        action_id: "queue_new_track",
      }),
    ]),
  ]);
};

let search = query => {
  Js.Promise.(
    switch (query) {
    | "" =>
      `Ok(
        Slack.Block.make([
          `Section(
            "You forgot to tell me what to search for\n*Example:* `search rebecca black friday`",
          ),
        ]),
      )
      |> resolve
    | query =>
      _search(query |> Js.Global.encodeURIComponent)
      |> then_(response => {
           let tracks = response |> WejayTrack.tracks;

           let message =
             switch (Belt.Array.length(tracks)) {
             | 0 => {j|Sorry, I couldn't find anything with *$query*|j}
             | _ => {j|Here are the results for *$query*|j}
             };

           let attachments =
             tracks
             ->Belt.Array.slice(~offset=0, ~len=5)
             ->Belt.Array.reduce([||], (acc, curr) =>
                 acc->Belt.Array.concat(createSearchAttachment(curr))
               );

           resolve(
             `Ok(
               Slack.Block.make([`Section(message)])
               ->Belt.Array.concat(attachments),
             ),
           );
         })
    }
  );
};
