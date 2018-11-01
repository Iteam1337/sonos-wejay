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
