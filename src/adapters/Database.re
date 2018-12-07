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
    timestamp: float_of_int(json |> field("timestamp", int)),
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

let openConnection = () =>
  MySql2.Connection.connect(
    ~host="mysql",
    ~port=3306,
    ~user="root",
    ~password="test",
    ~database="wejay",
    (),
  );

let insertTrack = (~uri, ~user) => {
  let spotifyId = Utils.spotifyId(uri);

  Js.Promise.(
    Spotify.getTrack(spotifyId)
    |> then_((track: Spotify.track) => {
         let conn = openConnection();
         let timestamp = Js.Date.now();

         let params =
           MySql2.Params.named(
             Json.Encode.(
               object_([
                 ("uri", string(uri)),
                 ("user_id", string(user)),
                 ("timestamp", Json.Encode.float(timestamp)),
                 ("artist", string(Spotify.buildArtist(track.artists))),
                 ("duration", int(track.duration)),
                 ("name", string(track.name)),
                 ("spotify_id", string(spotifyId)),
                 ("album", string(track.album.name)),
                 ("cover", string(track.album.images[0].url)),
               ])
             ),
           );

         MySql2.execute(
           conn,
           "INSERT INTO users (uri, user_id, timestamp, artist, duration, name, spotify_id, album, cover) VALUES (:uri, :user_id, :timestamp, :artist, :duration, :name, :spotify_id, :album, :cover)",
           Some(params),
           res => {
             switch (res) {
             | `Error(e) => Js.log2("ERROR: ", e)
             | `Select(select) => Js.log2("SELECT: ", select)
             | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
             };

             MySql2.Connection.close(conn);
           },
         );

         resolve(true);
       })
    |> ignore
  );
};

let formatTimestamp = timestamp => {
  let date = Js.Date.fromFloat(timestamp);
  DateFns.format(date, "YYYY-MM-DD");
};

let lastPlay = (uri, sendMessage) => {
  let conn = openConnection();

  let params =
    MySql2.Params.named(Json.Encode.(object_([("uri", string(uri))])));

  MySql2.execute(
    conn,
    "SELECT * FROM users WHERE uri=:uri ORDER BY timestamp DESC",
    Some(params),
    res => {
      switch (res) {
      | `Error(e) => Js.log2("ERROR: ", e)
      | `Select(select) =>
        let rows =
          select
          ->MySql2.Select.rows
          ->Belt.Array.map(item => item |> Decode.userRow);

        (
          switch (Belt.Array.length(rows)) {
          | 0 => "Sorry, I don't know who added this track"
          | 1 => Slack.userId(rows[0].userId) ++ " added this awesome track!"
          | _ =>
            "*This track has been added by*\n"
            ++ (
              rows->Belt.Array.mapWithIndex((i, {timestamp, userId}) =>
                Utils.listNumber(i)
                ++ Slack.userId(userId)
                ++ " on "
                ++ formatTimestamp(timestamp)
              )
              |> Js.Array.joinWith("\n")
            )
          }
        )
        |> sendMessage;

        ();
      | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
      };
      MySql2.Connection.close(conn);
    },
  );
};

let mostPlayed = sendMessage => {
  let conn = openConnection();

  MySql2.execute(
    conn,
    "SELECT DISTINCT artist, uri, name, COUNT(*) as count FROM users GROUP BY artist, name, uri ORDER BY count DESC LIMIT 10",
    None,
    res => {
      switch (res) {
      | `Error(e) => Js.log2("ERROR: ", e)
      | `Select(select) =>
        let rows =
          select
          ->MySql2.Select.rows
          ->Belt.Array.map(item => item |> Decode.mostPlayedRow);

        (
          switch (Belt.Array.length(rows)) {
          | 0 => "No plays :sad_panda:"
          | _ =>
            "*Most played*\n"
            ++ (
              rows->Belt.Array.mapWithIndex((i, {artist, count, name, uri}) =>
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
              |> Js.Array.joinWith("\n")
            )
          }
        )
        |> sendMessage;

        ();
      | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
      };

      MySql2.Connection.close(conn);
    },
  );
};

let toplist = sendMessage => {
  let conn = openConnection();

  MySql2.execute(
    conn,
    "SELECT user_id, count(*) as count FROM users GROUP BY user_id ORDER BY count(*) DESC",
    None,
    res => {
      switch (res) {
      | `Error(e) => Js.log2("ERROR: ", e)
      | `Select(select) =>
        let rows =
          select
          ->MySql2.Select.rows
          ->Belt.Array.map(item => item |> Decode.toplistRow);

        (
          switch (Belt.Array.length(rows)) {
          | 0 => "No plays :sad_panda:"
          | _ =>
            "*Toplist*\n"
            ++ (
              rows->Belt.Array.mapWithIndex((i, {userId, count}) =>
                Utils.listNumber(i)
                ++ Slack.userId(userId)
                ++ " ("
                ++ string_of_int(count)
                ++ ")"
              )
              |> Js.Array.joinWith("\n")
            )
          }
        )
        |> sendMessage;

        ();
      | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
      };

      MySql2.Connection.close(conn);
    },
  );
};
