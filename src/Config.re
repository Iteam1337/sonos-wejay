Dotenv.config();

let env = Node.Process.process##env;
let spotifyId = Js.Dict.unsafeGet(env, "SPOTIFY_ID");
let spotifySecret = Js.Dict.unsafeGet(env, "SPOTIFY_SECRET");
let slackToken = Js.Dict.unsafeGet(env, "SLACK_TOKEN");

let spotifyAuth =
  Node.Buffer.(
    fromStringWithEncoding(spotifyId ++ ":" ++ spotifySecret, `base64)
    |> toString
  );