open Lwt.Infix;

let handle = ({command, args}: Api.t) => {
  (
    switch (command) {
    | Search =>
      Cohttp_lwt_unix.Client.get(
        Uri.of_string("https://wejay.willcodefor.beer/search?q=" ++ args),
      )
    | _ =>
      Cohttp_lwt_unix.Client.post(
        Uri.of_string("https://wejay.willcodefor.beer/search?q=" ++ args),
      )
    }
  )
  >>= (
    ((_resp, body)) => {
      body |> Cohttp_lwt.Body.to_string >|= (body => body);
    }
  )
  |> Lwt_main.run;
};
