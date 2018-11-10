open Sonos;
open Js.Promise;

let library = (q, sendMessageWithAttachments) =>
  Sonos.device->searchMusicLibrary("tracks", q)
  |> then_(library => {
       let {items} = library |> SonosDecode.currentQueueResponse;

       items->Belt.Array.map(({album, artist, title, uri}) => {
         let text =
           "*"
           ++ Utils.artistAndTitle(~artist, ~title)
           ++ "*\n"
           ++ Belt.Option.getWithDefault(album, "");

         Utils.createAttachment(~text, ~uri, ());
       })
       |> sendMessageWithAttachments(
            "Searching in music library *" ++ q ++ "*",
          );

       library |> resolve;
     })
  |> catch(Utils.handleError("searchLibrary"))
  |> ignore;
