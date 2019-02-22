let formatTimestamp = timestamp => {
  let date = Js.Date.fromFloat(timestamp);
  DateFns.format(date, "YYYY-MM-DD");
};

let splitBy = (str, split) => str |> Js.String.split(split);
let joinWithNewline = arr => arr |> Js.Array.joinWith("\n");
let artistAndTitle = (~artist, ~title) => artist ++ " - " ++ title;
let listNumber = number => string_of_int(number + 1) ++ ". ";
let parseDuration = duration => duration *. 1000. |> Duration.parse;
let cleanFloat = value => value |> int_of_float |> string_of_int;

let parsedTrack = track =>
  track->Regex.replaceByRe(Regex.Patterns.removeSlackCommandBrackets, "");

let removeUser = text =>
  text->Regex.replaceByRe(Regex.Patterns.removeSlackUser, "");

let spotifyId = uri => uri->splitBy(":") |> (items => items[2]);

let createAttachment = (~text, ~uri, ~thumbUrl="", ()) => {
  "text": text,
  "callback_id": "queue",
  "thumb_url": thumbUrl,
  "actions": [|
    {"name": "track", "text": "Queue", "type": "button", "value": uri},
  |],
};

let parseTrackCopy = track =>
  track
  ->parsedTrack
  ->Regex.replaceByRe(Regex.Patterns.spotifyTrackURL, "spotify:track:")
  ->splitBy("\n");

let parsePlaylistCopy = track =>
  switch (Js.String.match(Regex.Patterns.spotifyPlaylistURL, track)) {
  | Some(matches) => [|
      "spotify:user:" ++ matches[1] ++ ":playlist:" ++ matches[2],
    |]
  | _ => [||]
  };

let parseSpotifyCopy = track =>
  Js.String.includes("track", track)
    ? parseTrackCopy(track) : parsePlaylistCopy(track);

let handleError = (parent, err) => {
  Js.log(err);
  Js.Promise.reject(Js.Exn.raiseError("Something went wrong in " ++ parent));
};

let isFriday = () =>
  switch (Js.Date.make() |> Js.Date.getDay) {
  | 5. => true
  | _ => false
  };
