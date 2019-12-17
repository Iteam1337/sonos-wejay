open Tablecloth;

let joinWithNewline = arr => arr |> Js.Array.joinWith("\n");
let artistAndTitle = (~artist, ~title) => artist ++ " - " ++ title;
let listNumber = number => string_of_int(number + 1) ++ ". ";
let cleanFloat = value => value |> int_of_float |> string_of_int;

let parsedTrack = track =>
  Regex.(
    track
    ->Js.String2.replaceByRe(Patterns.removeSlackCommandBrackets, "")
    ->Js.String2.replaceByRe(Patterns.queryParams, "")
    ->Js.String2.replaceByRe(Patterns.spotifyTrackURL, "spotify:track:")
  );

let removeUser = text =>
  text->Js.String2.replaceByRe(Regex.Patterns.removeSlackUser, "");

let parseTrackCopy = track => track->parsedTrack->Js.String2.split("\n");

let parsePlaylistCopy = track =>
  switch (Js.String.match(Regex.Patterns.spotifyPlaylistURL, track)) {
  | Some(matches) =>
    switch (matches->Array.get(~index=1)) {
    | Some(id) => [|"spotify:playlist:" ++ id|]
    | None => [||]
    }
  | _ => [||]
  };

let parseSpotifyCopy = track =>
  Js.String.includes("track", track)
    ? parseTrackCopy(track) : parsePlaylistCopy(track);

module Sonos = {
  let toSpotifyUri = sonosUri => {
    switch (sonosUri) {
    | Some(sonosUri) =>
      let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

      Js.String.(
        switch (includes("x-file", sonosUri), sonosUri |> match(spotifyUri)) {
        | (false, Some(match)) =>
          switch (match->Array.get(~index=0)) {
          | Some(uri) => Js.Global.decodeURIComponent(uri)
          | None => ""
          }
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
        ->Js.String2.replaceByRe(Patterns.removeSlackCommandBrackets, "")
        ->Js.String2.replaceByRe(Patterns.queryParams, "")
        ->Js.String2.replaceByRe(Patterns.spotifyTrackURL, "spotify:track:")
        ->Js.String2.replaceByRe(
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

    input
    ->List.getAt(~index=randomSongIndex)
    ->Option.withDefault(~default="");
  };
};
