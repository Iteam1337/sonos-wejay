open Sonos.Methods;
open Js.Promise;
open Slack.Block;

/* The Sonos device we send all commands to. Connect devices in the Sonos
 * Controller App to make it play everywhere. */
let device = Config.device;

let pause = () =>
  device->pause()
  |> then_(_ => `Ok(Simple.make(~message="Playback paused")) |> resolve)
  |> catch(_ =>
       `Ok(Simple.make(~message=Message.nothingIsPlaying)) |> resolve
     );

let next = () =>
  EasterEgg.Test.make(
    device->next()
    |> then_(_ =>
         `Ok(Simple.make(~message="Playing next track")) |> resolve
       )
    |> catch(_ =>
         `Ok(Simple.make(~message=Message.nothingIsPlaying)) |> resolve
       ),
  );

let previous = () =>
  EasterEgg.Test.make(
    device->previous()
    |> then_(_ =>
         `Ok(Simple.make(~message="Playing previous track")) |> resolve
       )
    |> catch(_ =>
         `Ok(Simple.make(~message=Message.nothingIsPlaying)) |> resolve
       ),
  );

let mute = isMuted =>
  device->setMuted(isMuted)
  |> then_(_ => {
       let message = isMuted ? "Muted speakers" : "Unmuted speakers";

       `Ok(Simple.make(~message)) |> resolve;
     });

let play = () =>
  Queue.queueWithFallback()
  |> then_(({items}: Sonos.Decode.currentQueueResponse) => {
       let message =
         switch (items->Belt.Array.length) {
         | 0 => Message.emptyQueue
         | _ =>
           device->play() |> then_(_ => resolve(true)) |> ignore;

           "Start playing!";
         };

       resolve(`Ok(Simple.make(~message)));
     });

let playTrack = trackNumber =>
  switch (trackNumber) {
  | "" =>
    `Ok(
      Simple.make(
        ~message="You forgot to add a track number\n*Example:* `playtrack 2`",
      ),
    )
    |> resolve
  | trackNumber =>
    EasterEgg.Test.make(
      device->selectTrack(trackNumber |> int_of_string)
      |> then_(_ =>
           Services.getCurrentTrack()
           |> then_(({artist, title}: Sonos.Decode.currentTrackResponse) => {
                Services.getPlayingState()
                |> then_((state: Sonos.Decode.currentPlayingState) => {
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
                  Simple.make(
                    ~message=
                      "*Playing track*\n"
                      ++ Utils.artistAndTitle(~artist, ~title),
                  ),
                )
                |> resolve;
              })
         )
      |> catch(_ =>
           Queue.queueWithFallback()
           |> then_(({items}: Sonos.Decode.currentQueueResponse) => {
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

                `Ok(Simple.make(~message)) |> resolve;
              })
         ),
    )
  };
