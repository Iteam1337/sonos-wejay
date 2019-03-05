open Sonos.Methods;
open Sonos.Decode;
open Js.Promise;

let device = Config.device;

let justResolve = inputFunction =>
  inputFunction
  |> then_(value => value |> resolve)
  |> catch(Utils.handleError("justResolve"))
  |> ignore;

let pause = () => device->pause()->justResolve;
let next = () => device->next()->justResolve;
let previous = () => device->previous()->justResolve;
let mute = isMuted => device->setMuted(isMuted)->justResolve;

let play = sendMessage =>
  Queue.queueWithFallback()
  |> then_(({items}) => {
       switch (items->Belt.Array.length) {
       | 0 => sendMessage(Messages.emptyQueue)
       | _ =>
         device->play() |> then_(_ => resolve(true)) |> ignore;
         sendMessage("Start playing!");
       };

       resolve(true);
     })
  |> ignore;

let playTrackNumber = (trackNumber, sendMessage) =>
  device->selectTrack(trackNumber |> int_of_string)
  |> then_(_ =>
       Services.getCurrentTrack()
       |> then_(({artist, title}) => {
            Services.getPlayingState()
            |> then_(state => {
                 switch (state) {
                 | Paused
                 | Stopped => play(sendMessage)
                 | Playing
                 | UnknownState => ()
                 };

                 resolve(true);
               })
            |> ignore;

            sendMessage(
              "*Playing track*\n" ++ Utils.artistAndTitle(~artist, ~title),
            )
            |> ignore;

            resolve(true);
          })
     )
  |> catch(_ => {
       Queue.queueWithFallback()
       |> then_(({items}) => {
            switch (items->Belt.Array.length) {
            | 0 =>
              sendMessage(
                "*Cannot play track "
                ++ trackNumber
                ++ "*\n"
                ++ Messages.emptyQueue,
              )
            | _ =>
              items
              ->Belt.Array.mapWithIndex((i, {artist, title}) =>
                  Utils.listNumber(i)
                  ++ Utils.artistAndTitle(~artist, ~title)
                )
              ->Utils.joinWithNewline
              ->(
                  tracks =>
                    sendMessage(
                      "*Cannot play track "
                      ++ trackNumber
                      ++ ". Here's the whole queue*\n"
                      ++ tracks,
                    )
                )
            };

            resolve(true);
          })
       |> ignore;

       resolve(false);
     })
  |> ignore;
