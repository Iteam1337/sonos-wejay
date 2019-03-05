open Sonos.Decode;
open Js.Promise;

let blame = sendMessage =>
  Services.getCurrentTrack()
  |> then_(({uri}) => {
       let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

       let uri =
         switch (
           Js.String.includes("x-file", uri),
           uri |> Js.String.match(spotifyUri),
         ) {
         | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
         | (true, None) => uri
         | _ => ""
         };

       DbService.handleLastPlayed(sendMessage) |> Database.lastPlay(uri);

       resolve("blame u");
     });

let blameU = () =>
  Services.getCurrentTrack()
  |> then_(({uri}) => {
       let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

       let uri =
         switch (
           Js.String.includes("x-file", uri),
           uri |> Js.String.match(spotifyUri),
         ) {
         | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
         | (true, None) => uri
         | _ => ""
         };

       resolve(`Ok(uri));
     });
