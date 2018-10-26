type userRow = {
  id: int,
  uri: string,
  userId: string,
  timestamp: int,
};

type userResponse = {rows: array(userRow)};

module Decode = {
  open Json.Decode;

  let userRow = json => {
    id: json |> field("id", int),
    uri: json |> field("uri", string),
    userId: json |> field("user_id", string),
    timestamp: json |> field("timestamp", int),
  };

  let decodeUser = json => json |> array(userRow);
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
          |> MySql2.Select.rows
          |> Array.map(item => item |> Decode.userRow);

        (
          switch (Array.length(rows)) {
          | 0 => "Sorry, I don't know who added this track"
          | 1 => "<@" ++ rows[0].userId ++ "> added this awesome track!"
          | _ =>
            "*This track has been added by*\n"
            ++ (
              rows
              |> Array.mapi((i, row) =>
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
