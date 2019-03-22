type t;

[@bs.val] external fromStringWithEncoding: string => t = "Buffer.from";
[@bs.send] external toString: (t, string) => string = "";

Dotenv.config();

let env = Node.Process.process##env;
let spotifyId = Js.Dict.unsafeGet(env, "SPOTIFY_ID");
let spotifySecret = Js.Dict.unsafeGet(env, "SPOTIFY_SECRET");
let slackToken = Js.Dict.unsafeGet(env, "SLACK_TOKEN");
let slackClientId = Js.Dict.unsafeGet(env, "SLACK_CLIENT_ID");
let slackClientSecret = Js.Dict.unsafeGet(env, "SLACK_CLIENT_SECRET");
let wejayIp = Js.Dict.unsafeGet(env, "WEJAY_IP");
let elasticUrl = Js.Dict.unsafeGet(env, "ELASTIC_URL");
let elasticLogUrl = Js.Dict.unsafeGet(env, "ELASTIC_LOG_URL");
let mostPlayedUrl = Js.Dict.unsafeGet(env, "MOST_PLAYED_URL");
let toplistUrl = Js.Dict.unsafeGet(env, "TOPLIST_URL");
let blameUrl = Js.Dict.unsafeGet(env, "BLAME_URL");

let spotifyAuth =
  fromStringWithEncoding(spotifyId ++ ":" ++ spotifySecret)
  ->toString("base64");

let device = Sonos.Methods.device(wejayIp);

device->Sonos.Methods.setSpotifyRegion(Sonos.Methods.regionEurope);
