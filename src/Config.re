Dotenv.config();

let env = Node.Process.process##env;
let spotifyToken = Js.Dict.unsafeGet(env, "SPOTIFY_TOKEN");
let slackToken = Js.Dict.unsafeGet(env, "SLACK_TOKEN");
