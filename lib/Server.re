open Opium.Std;
open Lwt.Infix;

let slack_event = post("/slack/event", _req => `String("Hello") |> respond');

let command =
  post("/command", req => {
    let json = req.body |> Rock.Body.to_string |> Ezjsonm.from_string;

    let%lwt result = json |> Api.decode |> Service.handle;

    `Json(result) |> respond';
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
