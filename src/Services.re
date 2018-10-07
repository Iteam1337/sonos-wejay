open Sonos;

let device = Sonos.device(Devices.Rickard.home);

device->setSpotifyRegion(regionEurope);

let handleResponse = (channel, message, value) => {
  Slack.sendSlackResponse(channel, message, ());
  value |> Js.Promise.resolve;
};

let queueTrack = (track, sendMessage) => {
  let parsedTrack = track |> Js.String.replaceByRe([%re "/(<|>)/g"], "");

  Js.Promise.(
    device->queue(parsedTrack)
    |> then_(value => {
         let response = value |> SonosDecode.queueResponse;

         sendMessage(
           "Queued track in position " ++ response.firstTrackNumberEnqueued,
           (),
         );

         response |> resolve;
       })
  )
  |> ignore;
};

let playTrack = () =>
  Js.Promise.(device->play() |> then_(value => value |> resolve)) |> ignore;

let pauseTrack = () =>
  Js.Promise.(device->pause() |> then_(value => value |> resolve)) |> ignore;

let clearPlaylist = () =>
  Js.Promise.(device->Sonos.flush() |> then_(value => value |> resolve))
  |> ignore;

let setVolume = (channel: string, volume: string) => {
  let sendMessage = handleResponse(channel);

  Js.Promise.(
    device->setVolume(volume |> float_of_string)
    |> then_(sendMessage("Volume set to " ++ volume))
  );
};

let mute = (channel: string, isMuted) => {
  let sendMessage = handleResponse(channel);

  Js.Promise.(device->setMuted(isMuted) |> then_(sendMessage("Muted")));
};

let previousTrack = (channel: string) => {
  let sendMessage = handleResponse(channel);

  Js.Promise.(device->previous() |> then_(sendMessage("Previous track")));
};

let nextTrack = (channel: string) => {
  let sendMessage = handleResponse(channel);

  Js.Promise.(device->next() |> then_(sendMessage("Next track")));
};

let currentQueue = () =>
  Js.Promise.(
    device->getQueue()
    |> then_(value => {
         Js.log(value);
         value |> resolve;
       })
  )
  |> ignore;

let currentTrack = () =>
  Js.Promise.(device->currentTrack() |> then_(value => value |> resolve));
