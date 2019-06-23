open Lwt.Infix;

let search = query =>
  Cohttp_lwt_unix.Client.get(
    Uri.of_string("https://wejay.willcodefor.beer/search?q=" ++ query),
  )
  >>= (
    ((_resp, body)) => {
      body
      |> Cohttp_lwt.Body.to_string
      >|= (
        body => {
          Decode.Spotify.(body |> Ezjsonm.from_string |> Tracks.of_json);
        }
      );
    }
  );
