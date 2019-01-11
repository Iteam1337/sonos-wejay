let spotifyTrack = [%re "/https:\\/\\/open.spotify.com\\/track\\//ig"];
let spotifyPlaylist = [%re
  "/https:\\/\\/open.spotify.com\\/user\\/(\\w+)\\/playlist\\/(\\w+)/"
];

let formatTimestamp = timestamp => {
  let date = Js.Date.fromFloat(timestamp);
  DateFns.format(date, "YYYY-MM-DD");
};

let artistAndTitle = (~artist, ~title) => artist ++ " - " ++ title;

let listNumber = number => string_of_int(number + 1) ++ ". ";

let parseDuration = duration => duration *. 1000. |> Duration.parse;
let cleanFloat = value => value |> int_of_float |> string_of_int;

let parsedTrack = track => Js.String.replaceByRe([%re "/(<|>)/g"], "", track);

let removeUser = text =>
  Js.String.replaceByRe([%re "/<@\\w+>\\s/g"], "", text);

let spotifyId = uri => uri |> Js.String.split(":") |> (items => items[2]);

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
  |> Js.String.replaceByRe([%re "/<|>/g"], "")
  |> Js.String.replaceByRe(spotifyTrack, "spotify:track:")
  |> Js.String.split("\n");

let parsePlaylistCopy = track =>
  switch (Js.String.match(spotifyPlaylist, track)) {
  | Some(matches) => [|
      "spotify:user:" ++ matches[1] ++ ":playlist:" ++ matches[2],
    |]
  | _ => [||]
  };

let parseSpotifyCopy = track =>
  Js.String.includes("track", track) ?
    parseTrackCopy(track) : parsePlaylistCopy(track);

let handleError = (parent, err) => {
  Js.log(err);
  Js.Promise.reject(Js.Exn.raiseError("Something went wrong in " ++ parent));
};

let help =
  [
    "*Queue and search*\n",
    "```",
    "currentqueue|getqueue|cq|gq _________ Display upcoming tracks",
    "fullqueue|fq ________________________ Display all tracks currently in the queue",
    "nowplaying|np _______________________ Display what's playing right now",
    "queue|q <Spotify URI> _______________ Queue a track using a Spotify URI",
    "search|s <your search string> _______ Search Spotify for a track",
    "library|l <your search string> ______ Search in music library",
    "```",
    "\n*Play control*",
    "```",
    "clear ______________________ Reset playlist",
    "mute _______________________ Mute",
    "next _______________________ Skip one track forward",
    "pause ______________________ Pause",
    "play _______________________ Play",
    "playtrack <trackNumber> ____ Play track number",
    "previous ___________________ Go back one track",
    "unmute _____________________ Unmute",
    "volume <0-100> _____________ Set volume, if no number is provided displays current volume",
    "```",
    "\n*Misc*",
    "```",
    "blame _____________ Who added the track?",
    "help ______________ Display this message",
    "mostplayed ________ List the most played tracks",
    "toplist ___________ List top Wejayers",
    "```",
    "\nThere's also some hidden easter eggs :hatching_chick:",
  ]
  |> String.concat("\n");

let unknownCommand = command =>
  [
    "Sorry, the command: *\"" ++ command ++ "\"* is not available.",
    "Type *\"help\"* in order to get all available commands! :face_with_monocle:",
  ]
  |> String.concat("\n");

let thisIsWejay = "This is Wejay!\nhttps://media.giphy.com/media/Ny4Ian52lZDz2/giphy.gif";
