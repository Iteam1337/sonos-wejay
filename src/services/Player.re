open Sonos;
open Js.Promise;

let justResolve = inputFunction =>
  inputFunction
  |> then_(value => value |> resolve)
  |> catch(Utils.handleError("justResolve"))
  |> ignore;

let pause = () => device->pause()->justResolve;
let play = () => device->play()->justResolve;
let next = () => device->next()->justResolve;
let previous = () => device->previous()->justResolve;
let mute = isMuted => device->setMuted(isMuted)->justResolve;

let playTrackNumber = (trackNumber, sendMessage) =>
  device->selectTrack(trackNumber |> int_of_string)
  |> then_(_ =>
       Services.getCurrentTrack()
       |> then_(({artist, title}) => {

            Services.getPlayingState()
            |> then_(state => {
              switch (state) {
              | Stopped => play()
              | Playing | UnknownState => ()
              }

              resolve(true)
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
       sendMessage(
         "*Cannot play track " ++ trackNumber ++ ". Here's the whole queue*",
       )
       |> ignore;

       device->getQueue()
       |> then_(queue => {
            let {items} = queue |> SonosDecode.currentQueueResponse;

            items->Belt.Array.mapWithIndex((i, {artist, title}) =>
              Utils.listNumber(i) ++ Utils.artistAndTitle(~artist, ~title)
            )
            |> Js.Array.joinWith("\n")
            |> sendMessage;

            resolve(true);
          })
       |> ignore;

       resolve(false);
     })
  |> ignore;
