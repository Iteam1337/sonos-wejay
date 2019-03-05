let run = () =>
  Js.Promise.(
    Services.getCurrentTrack()
    |> then_(({uri}: Sonos.Decode.currentTrackResponse) => {
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
       })
  );
