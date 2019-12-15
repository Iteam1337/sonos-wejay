open Js.Promise;

module Track = {
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

  let make = id => {
    WejayUtils.getTrack(id) |> then_(response => response |> track |> resolve);
  };
};

module Search = {
  let randomTracks = [
    "lamb of god walk with me in hell",
    "maini let me do your time",
    "nero promises (aka. the masseuse)",
    "rebecca black friday",
    "the mary onettes void",
    {j|kaj jåo nåo e ja jåo yolo ja nåo|j},
  ];

  let createAttachment =
      ({albumName, artist, cover, duration, name, uri}: Track.t) => {
    let trackDuration = Duration.make(duration);

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
          action_id: "queue_new_track",
          text: "Queue track",
          value: uri,
        }),
      ]),
    ]);
  };

  let make = query => {
    switch (query) {
    | "" =>
      Slack.Msg.make([
        `Section(
          "You forgot to tell me what to search for\n*Example:* `search "
          ++ Utils.RandomTrack.make(randomTracks)
          ++ "`",
        ),
      ])
    | query =>
      WejayUtils.search(query |> Js.Global.encodeURIComponent)
      |> then_(response => {
           let tracks = response |> Track.tracks;

           let message =
             switch (Belt.Array.length(tracks)) {
             | 0 => {j|Sorry, I couldn't find anything with *$query*|j}
             | _ => {j|Here are the results for *$query*|j}
             };

           let attachments =
             Belt.Array.(
               tracks
               ->slice(~offset=0, ~len=5)
               ->reduce([||], (acc, curr) =>
                   acc->concat(createAttachment(curr))
                 )
             );

           resolve(
             Ok(
               Slack.Result.Message(
                 Slack.Block.make([`Section(message)])
                 ->Belt.Array.concat(attachments),
               ),
             ),
           );
         })
    };
  };
};

module Playlist = {
  [@decco]
  type t = {owner: string};

  let parse = json => t_decode(json)->Parser.handle;

  let make = id => {
    WejayUtils.getPlaylist(id)
    |> then_(response => response |> parse |> resolve);
  };
};
