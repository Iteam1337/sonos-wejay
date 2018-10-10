type t;

[@bs.val] external fromStringWithEncoding: string => t = "Buffer.from";
[@bs.send] external toString: (t, string) => string = "";

Dotenv.config();

let env = Node.Process.process##env;
let spotifyId = Js.Dict.unsafeGet(env, "SPOTIFY_ID");
let spotifySecret = Js.Dict.unsafeGet(env, "SPOTIFY_SECRET");
let slackToken = Js.Dict.unsafeGet(env, "SLACK_TOKEN");

let spotifyAuth =
  fromStringWithEncoding(spotifyId ++ ":" ++ spotifySecret)
  ->toString("base64");
