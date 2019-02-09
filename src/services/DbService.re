type userRow = {
  id: int,
  uri: string,
  userId: string,
  timestamp: float,
};

type toplistRow = {
  count: int,
  userId: string,
};

type mostPlayedRow = {
  artist: option(string),
  count: int,
  name: option(string),
  uri: string,
};

module Decode = {
  open Json.Decode;

  let userRow = json => {
    id: json |> field("id", int),
    uri: json |> field("uri", string),
    userId: json |> field("user_id", string),
    timestamp: json |> field("timestamp", Json.Decode.float),
  };

  let toplistRow = json => {
    count: json |> field("count", int),
    userId: json |> field("user_id", string),
  };

  let mostPlayedRow = json => {
    artist: json |> optional(field("artist", string)),
    name: json |> optional(field("name", string)),
    count: json |> field("count", int),
    uri: json |> field("uri", string),
  };
};

let handleLastPlayed = (sendMessage, select) => {
  let rows =
    select->MySql2.Select.rows->Belt.Array.map(item => item |> Decode.userRow);

  (
    switch (Belt.Array.length(rows)) {
    | 0 => "Sorry, I don't know who added this track"
    | 1 => Slack.userId(rows[0].userId) ++ " added this awesome track!"
    | _ =>
      "*This track has been added by*\n"
      ++ rows
         ->Belt.Array.mapWithIndex((i, {timestamp, userId}) =>
             Utils.listNumber(i)
             ++ Slack.userId(userId)
             ++ " on "
             ++ Utils.formatTimestamp(timestamp)
           )
         ->Utils.joinWithNewline
    }
  )
  |> sendMessage;
};

let handleMostPlayed = (sendMessage, select) => {
  let rows =
    select
    ->MySql2.Select.rows
    ->Belt.Array.map(item => item |> Decode.mostPlayedRow);

  (
    switch (Belt.Array.length(rows)) {
    | 0 => "No plays :sad_panda:"
    | _ =>
      "*Most played*\n"
      ++ rows
         ->Belt.Array.mapWithIndex((i, {artist, count, name, uri}) =>
             Utils.listNumber(i)
             ++ (
               switch (artist) {
               | Some(a) => a ++ " - "
               | None => ""
               }
             )
             ++ Belt.Option.getWithDefault(name, uri)
             ++ " ("
             ++ string_of_int(count)
             ++ ")"
           )
         ->Utils.joinWithNewline
    }
  )
  |> sendMessage;
};

let handleToplist = (sendMessage, select) => {
  let rows =
    select
    ->MySql2.Select.rows
    ->Belt.Array.map(item => item |> Decode.toplistRow);

  (
    switch (Belt.Array.length(rows)) {
    | 0 => "No plays :sad_panda:"
    | _ =>
      "*Toplist*\n"
      ++ rows
         ->Belt.Array.mapWithIndex((i, {userId, count}) =>
             Utils.listNumber(i)
             ++ Slack.userId(userId)
             ++ " ("
             ++ string_of_int(count)
             ++ ")"
           )
         ->Utils.joinWithNewline
    }
  )
  |> sendMessage;
};
