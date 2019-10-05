open Js.Promise;

/* The Sonos device we send all commands to. Connect devices in the Sonos
 * Controller App to make it play everywhere. */
let device = Config.device;

let pause = () =>
  device->Sonos.Methods.PlayerControl.pause()
  |> then_(_ =>
       `Ok(Slack.Block.make([`Section("Playback paused")])) |> resolve
     )
  |> catch(_ =>
       `Ok(Slack.Block.make([`Section(Message.nothingIsPlaying)]))
       |> resolve
     );

let next = () =>
  EasterEgg.Test.make(
    device->Sonos.Methods.PlayerControl.next()
    |> then_(_ =>
         `Ok(Slack.Block.make([`Section("Playing next track")])) |> resolve
       )
    |> catch(_ =>
         `Ok(Slack.Block.make([`Section(Message.nothingIsPlaying)]))
         |> resolve
       ),
  );

let previous = () =>
  EasterEgg.Test.make(
    device->Sonos.Methods.PlayerControl.previous()
    |> then_(_ =>
         `Ok(Slack.Block.make([`Section("Playing previous track")]))
         |> resolve
       )
    |> catch(_ =>
         `Ok(Slack.Block.make([`Section(Message.nothingIsPlaying)]))
         |> resolve
       ),
  );

let mute = isMuted =>
  device->Sonos.Methods.PlayerControl.Volume.mute(isMuted)
  |> then_(_ => {
       let message = isMuted ? "Muted speakers" : "Unmuted speakers";

       `Ok(Slack.Block.make([`Section(message)])) |> resolve;
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

       resolve(`Ok(Slack.Block.make([`Section(message)])));
     });

let playTrack = trackNumber =>
  switch (trackNumber) {
  | "" =>
    `Ok(
      Slack.Block.make([
        `Section(
          "You forgot to add a track number\n*Example:* `playtrack 2`",
        ),
      ]),
    )
    |> resolve
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

                `Ok(
                  Slack.Block.make([
                    `Section(
                      "*Playing track*\n"
                      ++ Utils.artistAndTitle(~artist, ~title),
                    ),
                  ]),
                )
                |> resolve;
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

                `Ok(Slack.Block.make([`Section(message)])) |> resolve;
              })
         ),
    )
  };
