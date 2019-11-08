let splitBy = (str, split) => str |> Js.String.split(split);
let joinWithNewline = arr => arr |> Js.Array.joinWith("\n");
let artistAndTitle = (~artist, ~title) => artist ++ " - " ++ title;
let listNumber = number => string_of_int(number + 1) ++ ". ";
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

module Sonos = {
  let toSpotifyUri = sonosUri => {
    switch (sonosUri) {
    | Some(sonosUri) =>
      let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

      Js.String.(
        switch (includes("x-file", sonosUri), sonosUri |> match(spotifyUri)) {
        | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
        | (true, None) => sonosUri
        | _ => ""
        }
      );
    | None => ""
    };
  };
};

module Parse = {
  type t =
    | Track(string)
    | Playlist(string);

  let make = track => {
    let parsed =
      Regex.(
        track
        ->replaceByRe(Patterns.removeSlackCommandBrackets, "")
        ->replaceByRe(Patterns.queryParams, "")
        ->replaceByRe(Patterns.spotifyTrackURL, "spotify:track:")
        ->replaceByRe(
            Patterns.spotifyPlaylistURLWithoutUser,
            "spotify:playlist:",
          )
      );

    switch (parsed |> Js.String.includes(":playlist:")) {
    | false => Track(parsed)
    | true => Playlist(parsed)
    };
  };
};

module RandomTrack = {
  let make = input => {
    // Generate a seed based on the current time
    Random.init(int_of_float(Js.Date.now()));

    let randomSongIndex = Random.int(List.length(input));
    input->Belt.List.get(randomSongIndex)->Belt.Option.getWithDefault("");
  };
};
