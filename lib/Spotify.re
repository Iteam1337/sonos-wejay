open Lwt.Infix;
open Decode.Spotify;

module Utils = {
  let parse_spotify_uri = uri =>
    uri
    |> String.split_on_char(':')
    |> (
      strings =>
        List.nth_opt(strings, 2)
        |> (
          fun
          | Some(uri) => uri
          | _ => ""
        )
    );
};

let search = query =>
  Cohttp_lwt_unix.Client.get(
    Uri.of_string(
      Format.sprintf("https://wejay.willcodefor.beer/search?q=%s", query),
    ),
  )
  >>= (
    ((_resp, body)) => {
      body
      |> Cohttp_lwt.Body.to_string
      >|= (
        body => {
          Ok(`Tracks(body |> Ezjsonm.from_string |> Tracks.of_json));
        }
      );
    }
  );

let track = uri =>
  Cohttp_lwt_unix.Client.get(
    Uri.of_string(
      Format.sprintf(
        "https://wejay.willcodefor.beer/track?uri=%s",
        uri |> Utils.parse_spotify_uri,
      ),
    ),
  )
  >>= (
    ((_resp, body)) => {
      body
      |> Cohttp_lwt.Body.to_string
      >|= (
        body => {
          Ok(
            `Track(
              Ezjsonm.(
                body
                |> from_string
                |> (json => find(json, ["track"]) |> Track.of_json)
              ),
            ),
          );
        }
      );
    }
  );
