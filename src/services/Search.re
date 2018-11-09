open Sonos;
open Js.Promise;

let library = (q, sendMessageWithAttachments) => {
  let%Await library = Sonos.device->searchMusicLibrary("tracks", q);
  let {items} = library |> SonosDecode.currentQueueResponse;

  items
  ->Belt.Array.map(({album, artist, title, uri}) => {
      let text =
        "*"
        ++ Utils.artistAndTitle(~artist, ~title)
        ++ "*\n"
        ++ Belt.Option.getWithDefault(album, "");

      Utils.createAttachment(~text, ~uri, ());
    })
  |> sendMessageWithAttachments("Searching in music library *" ++ q ++ "*");

  library |> resolve;
};
