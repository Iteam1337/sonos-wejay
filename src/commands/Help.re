module Messages = {
  let queue = [
    `Section(
      {j|The _queue_ command adds a new track to the end of the queue.\nIt's used by copying a *URI* or *URL* of a track from Spotify.|j},
    ),
    `Section("*Shorthand version*: q"),
    `Section("*For example*:"),
    `Section("`q spotify:track:4abjt5hWugcmJucydd6eHL`"),
    `Section("`queue https://open.spotify.com/track/5fxDE80ZPA32JXjZN87pQJ`"),
  ];
};

let make = cmd => {
  (
    switch (cmd) {
    | Some(Commands.Queue) => Messages.queue
    | Some(_)
    | None => [`Section(Message.help)]
    }
  )
  |> Slack.Msg.make;
};
