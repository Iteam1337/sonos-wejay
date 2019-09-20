let formatDate = dateString =>
  dateString->Js.Date.fromString->DateFns.format("yyyy-MM-dd");

let formatTimestamp = timestamp =>
  timestamp->Js.Date.fromFloat->DateFns.format("yyyy-MM-dd");

let splitBy = (str, split) => str |> Js.String.split(split);
let joinWithNewline = arr => arr |> Js.Array.joinWith("\n");
let artistAndTitle = (~artist, ~title) => artist ++ " - " ++ title;
let listNumber = number => string_of_int(number + 1) ++ ". ";
let parseDuration = duration => duration *. 1000. |> Duration.parse;
let cleanFloat = value => value |> int_of_float |> string_of_int;

let parsedTrack = track =>
  Regex.(
    track
    ->replaceByRe(Patterns.removeSlackCommandBrackets, "")
    ->replaceByRe(Patterns.queryParams, "")
    ->replaceByRe(Patterns.spotifyTrackURL, "spotify:track:")
  );

let removeUser = text =>
  text->Regex.replaceByRe(Regex.Patterns.removeSlackUser, "");

let createAttachment = (~text, ~uri, ~thumbUrl="", ()) => {
  "text": text,
  "callback_id": "queue",
  "thumb_url": thumbUrl,
  "actions": [|
    {"name": "track", "text": "Queue", "type": "button", "value": uri},
  |],
};

let parseTrackCopy = track => track->parsedTrack->splitBy("\n");

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

let sonosUriToSpotifyUri = sonosUri => {
  let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

  Js.String.(
    switch (includes("x-file", sonosUri), sonosUri |> match(spotifyUri)) {
    | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
    | (true, None) => sonosUri
    | _ => ""
    }
  );
};
