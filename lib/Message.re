module Slack = {
  let track = ({albumName, artist, cover, name, _}: Decode.Spotify.Track.t) =>
    Ezjsonm.(
      dict([
        ("thumb_url", string(cover)),
        (
          "fields",
          dict([
            ("Artist", string(artist)),
            ("Title", string(name)),
            ("Album", string(albumName)),
          ]),
        ),
      ])
    );

  let tracks = tracks => Ezjsonm.(tracks |> list(track));

  let reply =
      (command: Command.t, payload: list(Decode.Spotify.Track.t), _event) => {
    switch (command) {
    | Search => tracks(payload)
    | _ => tracks(payload)
    };
  };
};

module Api = {
  let tracks = json => Decode.Spotify.Tracks.(json |> to_json);

  let reply = (command, payload) => {
    Command.(
      switch (command) {
      | Search => tracks(payload)
      | _ => tracks(payload)
      }
    );
  };
};

type t = [ | `Api | `Slack(Decode.Slack.Event.event)];

let reply = (reply_type, command, payload) =>
  switch (reply_type) {
  | `Api => Api.reply(command, payload)
  | `Slack(event) => Slack.reply(command, payload, event)
  };
