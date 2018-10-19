type sonosDevice;

type queueResponse = {
  firstTrackNumberEnqueued: string,
  newQueueLength: string,
  numTracksAdded: string,
};

type currentQueue = {
  album: string,
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
  album: string,
  albumArtURI: string,
  albumArtURL: string,
  artist: string,
  duration: float,
  position: float,
  queuePosition: float,
  title: string,
  uri: string,
};

module SonosDecode = {
  open Json.Decode;

  let queueResponse = json => {
    firstTrackNumberEnqueued:
      json |> field("FirstTrackNumberEnqueued", string),
    newQueueLength: json |> field("NewQueueLength", string),
    numTracksAdded: json |> field("NumTracksAdded", string),
  };

  let currentQueue = json => {
    "album": json |> optional(field("album", string)),
    "albumArtURI": json |> field("albumArtURI", string),
    "artist": json |> field("artist", string),
    "title": json |> field("title", string),
    "uri": json |> field("uri", string),
  };

  let currentQueueResponse = json => {
    "returned": json |> field("returned", string),
    "total": json |> field("total", string),
    "items": json |> field("items", array(currentQueue)),
  };

  let currentTrackResponse = json => {
    "album": json |> optional(field("album", string)),
    "albumArtURI": json |> field("albumArtURI", string),
    "albumArtURL": json |> field("albumArtURL", string),
    "artist": json |> field("artist", string),
    "duration": json |> field("duration", Json.Decode.float),
    "position": json |> field("position", Json.Decode.float),
    "queuePosition": json |> field("queuePosition", Json.Decode.float),
    "title": json |> field("title", string),
    "uri": json |> field("uri", string),
  };
};

[@bs.new] [@bs.module "sonos"]
external device: string => sonosDevice = "Sonos";

[@bs.module "sonos"] [@bs.scope "SpotifyRegion"]
external regionEurope: string = "EU";

[@bs.send] external setSpotifyRegion: (sonosDevice, string) => unit = "";

[@bs.send]
external getVolume: (sonosDevice, unit) => Js.Promise.t(float) = "";
[@bs.send]
external currentTrack: (sonosDevice, unit) => Js.Promise.t('a) = "";
[@bs.send]
external queue: (sonosDevice, string, int) => Js.Promise.t('a) = "";
[@bs.send] external play: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external pause: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external flush: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external setVolume: (sonosDevice, float) => Js.Promise.t('a) = "";
[@bs.send] external getQueue: (sonosDevice, unit) => Js.Promise.t('a) = "";
[@bs.send] external setMuted: (sonosDevice, bool) => Js.Promise.t(bool) = "";
[@bs.send] external next: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send] external previous: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send]
external selectQueue: (sonosDevice, unit) => Js.Promise.t(bool) = "";
[@bs.send]
external searchMusicLibrary: (sonosDevice, string, string) => Js.Promise.t('a) =
  "";
