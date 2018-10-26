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

let help =
  "*Queue and search*\n"
  ++ "```"
  ++ "currentqueue|getqueue _______________ Display upcoming tracks\n"
  ++ "nowplaying|np _______________________ Display what's playing right now\n"
  ++ "queue|q <Spotify URI> _______________ Queue a track using a Spotify URI\n"
  ++ "search|s <your search string> _______ Search Spotify for a track\n"
  ++ "library|l <your search string> ______ Search in music library"
  ++ "```"
  ++ "\n\n*Play control*\n"
  ++ "```"
  ++ "clear ______________________ Reset playlist\n"
  ++ "mute _______________________ Mute\n"
  ++ "next _______________________ Skip one track forward\n"
  ++ "pause ______________________ Pause\n"
  ++ "play _______________________ Play\n"
  ++ "playtrack <trackNumber> ____ Play track number\n"
  ++ "previous ___________________ Go back one track\n"
  ++ "unmute _____________________ Unmute\n"
  ++ "volume <0-100> _____________ Set volume, if no number is provided displays current volume"
  ++ "```"
  ++ "\n\n*Misc*\n"
  ++ "```"
  ++ "blame _____________ Who added the track?\n"
  ++ "help ______________ Display this message\n"
  ++ "```"
  ++ "\n\nThere's also some hidden easter eggs :hatching_chick:";
