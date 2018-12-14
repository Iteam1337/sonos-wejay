type sonosDevice;

type queueResponse = {
  firstTrackNumberEnqueued: string,
  newQueueLength: string,
  numTracksAdded: string,
};

type currentQueue = {
  album: option(string),
  albumArtURI: string,
  artist: string,
  title: string,
  uri: string,
};

type currentQueueResponse = {
  returned: string,
  total: string,
  items: array(currentQueue),
};

type currentTrackResponse = {
  album: option(string),
  albumArtURI: string,
  albumArtURL: string,
  artist: string,
  duration: float,
  position: float,
  queuePosition: float,
  title: string,
  uri: string,
};

type currentPlayingState =
  | Stopped
  | Playing
  | UnknownState;

module SonosDecode = {
  open Json.Decode;

  let queueResponse = json => {
    firstTrackNumberEnqueued:
      json |> field("FirstTrackNumberEnqueued", string),
    newQueueLength: json |> field("NewQueueLength", string),
    numTracksAdded: json |> field("NumTracksAdded", string),
  };

  let currentQueue = json => {
    album: json |> optional(field("album", string)),
    albumArtURI: json |> field("albumArtURI", string),
    artist: json |> field("artist", string),
    title: json |> field("title", string),
    uri: json |> field("uri", string),
  };

  let currentQueueResponse = json => {
    returned: json |> field("returned", string),
    total: json |> field("total", string),
    items: json |> field("items", array(currentQueue)),
  };

  let getWithDefault = field => Belt.Option.getWithDefault(field, "");

  let currentTrackResponse = json => {
    album: json |> optional(field("album", string)),
    albumArtURI:
      getWithDefault(json |> optional(field("albumArtURI", string))),
    albumArtURL:
      getWithDefault(json |> optional(field("albumArtURL", string))),
    artist: getWithDefault(json |> optional(field("artist", string))),
    duration: json |> field("duration", Json.Decode.float),
    position: json |> field("position", Json.Decode.float),
    queuePosition: json |> field("queuePosition", Json.Decode.float),
    title: getWithDefault(json |> optional(field("title", string))),
    uri: getWithDefault(json |> optional(field("uri", string))),
  };

  let currentPlayingState = currentState =>
    switch (currentState) {
    | "stopped" => Stopped
    | "playing" => Playing
    | _ => UnknownState
    };
};

[@bs.new] [@bs.module "sonos"]
external speaker: string => sonosDevice = "Sonos";

/* Group Control */
[@bs.deriving abstract]
type groupReqArgs = {
  [@bs.as "InstanceID"]
  instanceId: int,
  [@bs.as "DesiredVolume"]
  volume: string,
};

type sonosGroup = {
  .
  [@bs.meth] "_request": (string, groupReqArgs) => Js.Promise.t(Js.Json.t),
};

[@bs.new] [@bs.module "sonos"] [@bs.scope "Services"]
external groupRenderingControl: string => sonosGroup = "GroupRenderingControl";

/* Methods */

[@bs.module "sonos"] [@bs.scope "SpotifyRegion"]
external regionEurope: string = "EU";

[@bs.send] external setSpotifyRegion: (sonosDevice, string) => unit = "";

[@bs.send]
external getVolume: (sonosDevice, unit) => Js.Promise.t(float) = "";
[@bs.send]
external currentTrack: (sonosDevice, unit) => Js.Promise.t('a) = "";
[@bs.send]
external queue: (sonosDevice, string, int) => Js.Promise.t('a) = "";
[@bs.send]
external queueAsLast: (sonosDevice, string) => Js.Promise.t('a) = "queue";
[@bs.send] external play: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external pause: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external flush: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external setVolume: (sonosDevice, float) => Js.Promise.t('a) = "";
[@bs.send]
external getCurrentState: (sonosDevice, unit) => Js.Promise.t(string) = "";
[@bs.send] external getQueue: (sonosDevice, unit) => Js.Promise.t('a) = "";
[@bs.send] external setMuted: (sonosDevice, bool) => Js.Promise.t(bool) = "";
[@bs.send] external next: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external previous: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send]
external selectQueue: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send]
external searchMusicLibrary: (sonosDevice, string, string) => Js.Promise.t('a) =
  "";
[@bs.send]
external selectTrack: (sonosDevice, int) => Js.Promise.t(bool) = "";

let device = speaker(Config.wejayIp);
