[@bs.send]
external removeMultipleTracks:
  (Sonos.Methods.sonosDevice, int, int) => Js.Promise.t('a) =
  "removeTracksFromQueue";

[@bs.send]
external on: (Sonos.Methods.sonosDevice, string, Js.Json.t => unit) => unit =
  "on";

module CurrentTrack = {
  [@decco]
  type t = {
    title: string,
    artist: string,
    album: string,
    albumArtURI: string,
    uri: string,
    duration: int,
    queuePosition: int,
  };

  let make = json => {
    switch (t_decode(json)) {
    | Belt.Result.Ok(t) => t
    | Belt.Result.Error(_) => failwith("Could not parse current track")
    };
  };
};

module NextTrack = {
  [@decco]
  type t = {
    title: string,
    artist: string,
    album: string,
    albumArtURI: string,
    uri: string,
  };

  let make = json => {
    switch (t_decode(json)) {
    | Belt.Result.Ok(t) => t
    | Belt.Result.Error(_) => failwith("Could not parse current track")
    };
  };
};
