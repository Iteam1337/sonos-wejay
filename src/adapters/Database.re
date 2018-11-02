type userRow = {
  id: int,
  uri: string,
  userId: string,
  timestamp: int,
};

type toplistRow = {
  count: int,
  userId: string,
};

type mostPlayedRow = {
  count: int,
  uri: string,
};

module Decode = {
  open Json.Decode;

  let userRow = json => {
    id: json |> field("id", int),
    uri: json |> field("uri", string),
    userId: json |> field("user_id", string),
    timestamp: json |> field("timestamp", int),
  };

  let toplistRow = json => {
    count: json |> field("count", int),
    userId: json |> field("user_id", string),
  };

  let mostPlayedRow = json => {
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

let insertTrack = (~uri, ~user, ~time) => {
  let conn = openConnection();

  let params =
    MySql2.Params.named(
      Json.Encode.(
        object_([
          ("uri", string(uri)),
          ("user_id", string(user)),
          ("timestamp", int(time)),
        ])
      ),
    );

  MySql2.execute(
    conn,
    "INSERT INTO users (uri, user_id, timestamp) VALUES (:uri, :user_id, :timestamp)",
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
          | 1 => "<@" ++ rows[0].userId ++ "> added this awesome track!"
          | _ =>
            "*This track has been added by*\n"
            ++ (
              rows->Belt.Array.mapWithIndex((i, row) =>
                (i + 1 |> string_of_int)
                ++ ". <@"
                ++ row.userId
                ++ "> on "
                ++ DateFns.format(
                     row.timestamp |> string_of_int,
                     "YYYY-MM-DD HH:mm",
                   )
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
    "SELECT DISTINCT uri, COUNT(*) as count FROM users GROUP BY uri ORDER BY count DESC LIMIT 10",
    None,
    res => {
      switch (res) {
      | `Error(e) => Js.log2("ERROR: ", e)
      | `Select(select) =>
        let rows =
          select
          ->MySql2.Select.rows
          ->Belt.Array.map(item => item |> Decode.mostPlayedRow);

        let spotifyId = row =>
          row.uri |> Js.String.split(":") |> (items => items[2]);

        Js.Promise.(
          all(rows->Belt.Array.map(row => Spotify.getTrack(spotifyId(row))))
          |> then_((tracks: array(Spotify.track)) => {
               (
                 switch (Belt.Array.length(tracks)) {
                 | 0 => "No plays :san_panda:"
                 | _ =>
                   "*Most played*\n"
                   ++ (
                     tracks->Belt.Array.mapWithIndex((i, track) =>
                       (i + 1 |> string_of_int)
                       ++ ". "
                       ++ Spotify.buildArtist(track.artists)
                       ++ " - "
                       ++ track.name
                       ++ " ("
                       ++ string_of_int(rows[i].count)
                       ++ ")"
                     )
                     |> Js.Array.joinWith("\n")
                   )
                 }
               )
               |> sendMessage;

               resolve(tracks);
             })
        )
        |> ignore;

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
          | 0 => "No plays :san_panda:"
          | _ =>
            "*Toplist*\n"
            ++ (
              rows->Belt.Array.mapWithIndex((i, row) =>
                (i + 1 |> string_of_int)
                ++ ". <@"
                ++ row.userId
                ++ "> ("
                ++ string_of_int(row.count)
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
