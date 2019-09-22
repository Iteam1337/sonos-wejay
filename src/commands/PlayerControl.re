open Sonos.Methods;
open Js.Promise;

/* The Sonos device we send all commands to. Connect devices in the Sonos
 * Controller App to make it play everywhere. */
let device = Config.device;

let pause = () =>
  device->pause() |> then_(_ => resolve(`Ok("Playback paused")));

let next = () =>
  EasterEgg.isEasterEgg()
  |> then_(isEasterEgg =>
       isEasterEgg
         ? resolve(`Ok(Message.cantSkipEasterEgg))
         : device->next() |> then_(_ => resolve(`Ok("Playing next track")))
     );

let previous = () =>
  EasterEgg.isEasterEgg()
  |> then_(isEasterEgg =>
       isEasterEgg
         ? resolve(`Ok(Message.cantSkipEasterEgg))
         : device->previous()
           |> then_(_ => resolve(`Ok("Playing previous track")))
     );

let mute = isMuted =>
  device->setMuted(isMuted)
  |> then_(_ =>
       resolve(`Ok(isMuted ? "Muted speakers" : "Unmuted speakers"))
     );

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

       resolve(`Ok(message));
     });

let playTrack = trackNumber =>
  EasterEgg.isEasterEgg()
  |> then_(isEasterEgg =>
       isEasterEgg
         ? resolve(`Ok(Message.cantSkipEasterEgg))
         : device->selectTrack(trackNumber |> int_of_string)
           |> then_(_ =>
                Services.getCurrentTrack()
                |> then_(
                     ({artist, title}: Sonos.Decode.currentTrackResponse) => {
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
                       "*Playing track*\n"
                       ++ Utils.artistAndTitle(~artist, ~title),
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

                     `Ok(message) |> resolve;
                   })
              )
     );
