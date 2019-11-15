open Js.Promise;

/* The Sonos device we send all commands to. Connect devices in the Sonos
 * Controller App to make it play everywhere. */
let device = Config.device;

let pause = () =>
  device->Sonos.Methods.PlayerControl.pause()
  |> then_(_ => Slack.Msg.make([`Section("Playback paused")]))
  |> catch(_ => Slack.Msg.make([`Section(Message.nothingIsPlaying)]));

let next = () =>
  EasterEgg.Test.make(
    device->Sonos.Methods.PlayerControl.next()
    |> then_(_ => Slack.Msg.make([`Section("Playing next track")]))
    |> catch(_ => Slack.Msg.make([`Section(Message.nothingIsPlaying)])),
  );

let previous = () =>
  EasterEgg.Test.make(
    device->Sonos.Methods.PlayerControl.previous()
    |> then_(_ => Slack.Msg.make([`Section("Playing previous track")]))
    |> catch(_ => Slack.Msg.make([`Section(Message.nothingIsPlaying)])),
  );

let mute = isMuted =>
  device->Sonos.Methods.PlayerControl.Volume.mute(isMuted)
  |> then_(_ => {
       let message = isMuted ? "Muted speakers" : "Unmuted speakers";

       Slack.Msg.make([`Section(message)]);
     });

let play = () =>
  Queue.queueWithFallback()
  |> then_(({items}: Sonos.Decode.CurrentQueue.t) => {
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
     });

let playTrack = trackNumber =>
  switch (trackNumber) {
  | "" =>
    Slack.Msg.make([
      `Section("You forgot to add a track number\n*Example:* `playtrack 2`"),
    ])
  | trackNumber =>
    EasterEgg.Test.make(
      device->Sonos.Methods.Track.select(trackNumber |> int_of_string)
      |> then_(_ =>
           Services.getCurrentTrack()
           |> then_(({artist, title}: Sonos.Decode.CurrentTrack.t) => {
                Services.getPlayingState()
                |> then_((state: Sonos.Decode.CurrentPlayState.t) => {
                     switch (state) {
                     | Paused
                     | Stopped => play() |> ignore
                     | Playing
                     | UnknownState => ()
                     };

                     resolve(true);
                   })
                |> ignore;

                Slack.Msg.make([
                  `Section(
                    "*Playing track*\n"
                    ++ Utils.artistAndTitle(~artist, ~title),
                  ),
                ]);
              })
         )
      |> catch(_ =>
           Queue.queueWithFallback()
           |> then_(({items}: Sonos.Decode.CurrentQueue.t) => {
                let message =
                  switch (items->Belt.Array.length) {
                  | 0 =>
                    "*Cannot play track "
                    ++ trackNumber
                    ++ "*\n"
                    ++ Message.emptyQueue
                  | _ =>
                    items
                    ->Belt.Array.mapWithIndex((i, {artist, title}) =>
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
         ),
    )
  };

let playLatestTrack = () =>
  Queue.queueWithFallback()
  |> then_(({items}: Sonos.Decode.CurrentQueue.t) => {
       let lastTrack = items->Belt.Array.length;
       playTrack(string_of_int(lastTrack));
     });
