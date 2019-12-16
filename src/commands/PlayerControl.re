open Js.Promise;
open Tablecloth;

/* The Sonos device we send all commands to. Connect devices in the Sonos
 * Controller App to make it play everywhere. */
let device = Config.device;

let pause = () =>
  device->Sonos.Methods.PlayerControl.pause()
  |> then_(_ => Slack.Msg.make([`Section("Playback paused")]))
  |> catch(_ => Slack.Msg.make([`Section(Message.nothingIsPlaying)]));

let next = () => {
  let%Async isEasterEgg = EasterEgg.Test.make();

  switch (isEasterEgg) {
  | Ok(_) =>
    device->Sonos.Methods.PlayerControl.next()
    |> then_(_ => {Slack.Msg.make([`Section("Playing next track")])})
    |> catch(_ => Slack.Msg.make([`Section(Message.nothingIsPlaying)]))
  | Error(msg) => Slack.Msg.make([`Section(msg)])
  };
};

let previous = () => {
  let%Async isEasterEgg = EasterEgg.Test.make();

  switch (isEasterEgg) {
  | Ok(_) =>
    device->Sonos.Methods.PlayerControl.previous()
    |> then_(_ => Slack.Msg.make([`Section("Playing previous track")]))
    |> catch(_ => Slack.Msg.make([`Section(Message.nothingIsPlaying)]))
  | Error(msg) => Slack.Msg.make([`Section(msg)])
  };
};

let mute = isMuted => {
  let%Async _ = device->Sonos.Methods.PlayerControl.Volume.mute(isMuted);
  let message = isMuted ? "Muted speakers" : "Unmuted speakers";

  Slack.Msg.make([`Section(message)]);
};

let play = () => {
  let%Async {items} = Queue.queueWithFallback();

  let message =
    switch (items->Belt.Array.length) {
    | 0 => Message.emptyQueue
    | _ =>
      device->Sonos.Methods.PlayerControl.play()
      |> then_(_ => resolve(true))
      |> ignore;

      "Start playing!";
    };

  Slack.Msg.make([`Section(message)]);
};

let playTrack = trackNumber => {
  switch (trackNumber) {
  | "" =>
    Slack.Msg.make([
      `Section("You forgot to add a track number\n*Example:* `playtrack 2`"),
    ])
  | trackNumber =>
    let%Async isEasterEgg = EasterEgg.Test.make();

    switch (isEasterEgg) {
    | Error(msg) => Slack.Msg.make([`Section(msg)])
    | Ok(_) =>
      device->Sonos.Methods.Track.select(trackNumber |> int_of_string)
      |> then_(_ => {
           let%Async {artist, title} = Services.getCurrentTrack();
           let%Async state = Services.getPlayingState();

           switch (state) {
           | Paused
           | Stopped => play() |> ignore
           | Playing
           | UnknownState => ()
           };

           Slack.Msg.make([
             `Section(
               "*Playing track*\n" ++ Utils.artistAndTitle(~artist, ~title),
             ),
           ]);
         })
      |> catch(_ => {
           let%Async {items} = Queue.queueWithFallback();

           let message =
             switch (Array.length(items)) {
             | 0 =>
               "*Cannot play track "
               ++ trackNumber
               ++ "*\n"
               ++ Message.emptyQueue
             | _ =>
               items
               ->Array.mapWithIndex(~f=(i, {artist, title}) =>
                   Utils.listNumber(i)
                   ++ Utils.artistAndTitle(~artist, ~title)
                 )
               ->Utils.joinWithNewline
               ->(
                   tracks =>
                     "*Cannot play track "
                     ++ trackNumber
                     ++ ". Here's the whole queue*\n"
                     ++ tracks
                 )
             };

           Slack.Msg.make([`Section(message)]);
         })
    };
  };
};

let playLatestTrack = () => {
  let%Async {items} = Queue.queueWithFallback();

  items->Array.length->String.fromInt->playTrack;
};
