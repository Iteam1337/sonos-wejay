open Sonos.Decode;
open Js.Promise;

let blameCurrent = () => {
  Services.getCurrentTrack()
  |> then_(({uri}) => {
       let spotifyUri = [%re "/spotify%3atrack%3a[a-z0-9]+/ig"];

       (
         switch (
           Js.String.includes("x-file", uri),
           uri |> Js.String.match(spotifyUri),
         ) {
         | (false, Some(match)) => Js.Global.decodeURIComponent(match[0])
         | (true, None) => uri
         | _ => ""
         }
       )
       |> resolve;
     });
};

let blame = sendMessage =>
  blameCurrent()
  |> then_(uri => {
       DbService.handleLastPlayed(sendMessage) |> Database.lastPlay(uri);

       resolve(true);
     })
  |> ignore;
