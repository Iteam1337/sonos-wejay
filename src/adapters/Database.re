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

let insertTrack = (~uri, ~user, ~time) => {
  let conn =
    MySql2.Connection.connect(
      ~host="127.0.0.1",
      ~port=3306,
      ~user="root",
      ~password="test",
      ~database="wejay",
      (),
    );

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
  let conn =
    MySql2.Connection.connect(
      ~host="127.0.0.1",
      ~port=3306,
      ~user="root",
      ~password="test",
      ~database="wejay",
      (),
    );

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
        let resp =
          select
          |> MySql2.Select.rows
          |> Array.map(item => item |> Decode.userRow);

        sendMessage("<@" ++ resp[0].userId ++ "> added this awesome track!");

        ();
      | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
      };
      MySql2.Connection.close(conn);
    },
  );
};
