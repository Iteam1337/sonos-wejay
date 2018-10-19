let trackInfo = track => track##artist ++ " - " ++ track##title;
let parseDuration = duration => duration *. 1000. |> Duration.parse;
let cleanFloat = value => value |> int_of_float |> string_of_int;

let parsedTrack = track =>
  track |> Js.String.replaceByRe([%re "/(<|>)/g"], "");

let createAttachment = (~text, ~uri, ~thumbUrl="", ()) => {
  "text": text,
  "callback_id": "queue",
  "thumb_url": thumbUrl,
  "actions": [|
    {"name": "track", "text": "Queue", "type": "button", "value": uri},
  |],
};

let handleError = (parent, err) => {
  Js.log(err);
  Js.Promise.reject(Js.Exn.raiseError("Something went wrong in " ++ parent));
};
