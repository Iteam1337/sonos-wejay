type sonosDevice;

type queueResponse = {
  firstTrackNumberEnqueued: string,
  newQueueLength: string,
  numTracksAdded: string,
};

module SonosDecode = {
  let queueResponse = json =>
    Json.Decode.{
      firstTrackNumberEnqueued:
        json |> field("FirstTrackNumberEnqueued", string),
      newQueueLength: json |> field("NewQueueLength", string),
      numTracksAdded: json |> field("NumTracksAdded", string),
    };
};

[@bs.new] [@bs.module "sonos"]
external device: string => sonosDevice = "Sonos";

[@bs.module "sonos"] [@bs.scope "SpotifyRegion"]
external regionEurope: string = "EU";

[@bs.send] external setSpotifyRegion: (sonosDevice, string) => unit = "";

[@bs.send]
external currentTrack: (sonosDevice, unit) => Js.Promise.t('a) = "";
[@bs.send] external queue: (sonosDevice, string) => Js.Promise.t('a) = "";
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
