open Opium.Std;
open Lwt.Infix;

let slack_event = post("/slack/event", _req => `String("Hello") |> respond');

let command =
  post("/command", req => {
    /*
      * Use this?
      *
      * Returns Ezjsonm.t
      * let json = Lwt_main.run(req |> App.json_of_body_exn);
     */

    let json = req.body |> Rock.Body.to_string |> Ezjsonm.from_string;

    let result = json |> Api.decode |> Service.handle;

    let as_json =
      Decode.Spotify.(
        result |> Ezjsonm.from_string |> Tracks.of_json |> Tracks.to_json
      );

    `Json(as_json) |> respond';
  });

let wejay = get("/", _req => `Html("<h1>This is Wejay</h1>") |> respond');

let run = () => {
  let app = App.empty |> slack_event |> command |> wejay |> App.run_command';

  switch (app) {
  | `Ok(app) =>
    let s =
      Logging.log_reporter()
      >>= (
        r => {
          Logs.set_reporter(r);
          Logs.set_level(Some(Logs.Info));
          app;
        }
      );

    Lwt.async(() => s >>= (_ => Lwt.return_unit));
    Lwt_main.run(fst(Lwt.wait()));
  | `Error => exit(1)
  | `Not_running => exit(0)
  };
};
