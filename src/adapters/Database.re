let openConnection = () =>
  MySql2.Connection.connect(
    ~host="mysql",
    ~port=3306,
    ~user="root",
    ~password="test",
    ~database="wejay",
    (),
  );

let runQuery = (~query, ~params=None, ~callback, ()) => {
  let conn = openConnection();

  conn->MySql2.execute(query, params, res => res->callback(conn));
};

let insertTrack = (~uri, ~user) => {
  let spotifyId = Utils.spotifyId(uri);

  Js.Promise.(
    Spotify.getTrack(spotifyId)
    |> then_((track: Spotify.track) => {
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

         runQuery(
           ~query=
             "INSERT INTO users (uri, user_id, timestamp, artist, duration, name, spotify_id, album, cover) VALUES (:uri, :user_id, :timestamp, :artist, :duration, :name, :spotify_id, :album, :cover)",
           ~params=Some(params),
           ~callback=
             (res, conn) => {
               switch (res) {
               | `Error(e) => Js.log2("ERROR: ", e)
               | `Select(select) => Js.log2("SELECT: ", select)
               | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
               };

               MySql2.Connection.close(conn);
             },
           (),
         );

         resolve(true);
       })
    |> ignore
  );
};

let lastPlay = (uri, cb) => {
  let params =
    MySql2.Params.named(Json.Encode.(object_([("uri", string(uri))])));

  runQuery(
    ~query="SELECT * FROM users WHERE uri=:uri ORDER BY timestamp DESC",
    ~params=Some(params),
    ~callback=
      (res, conn) => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) => cb(select)
        | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
        };
        MySql2.Connection.close(conn);
      },
    (),
  );
};

let mostPlayed = cb =>
  runQuery(
    ~query=
      "SELECT DISTINCT artist, uri, name, COUNT(*) as count FROM users GROUP BY artist, name, uri ORDER BY count DESC LIMIT 10",
    ~callback=
      (res, conn) => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) => cb(select)
        | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
        };

        MySql2.Connection.close(conn);
      },
    (),
  );

let toplist = cb =>
  runQuery(
    ~query=
      "SELECT user_id, count(*) as count FROM users GROUP BY user_id ORDER BY count(*) DESC",
    ~callback=
      (res, conn) => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) => cb(select)
        | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
        };

        MySql2.Connection.close(conn);
      },
    (),
  );
