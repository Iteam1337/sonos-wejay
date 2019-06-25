open Opium.Std;
open Lwt.Infix;

let slack_event =
  post("/slack/event", req => {
    let json = req.body |> Rock.Body.to_string |> Ezjsonm.from_string;

    let%lwt result =
      json
      |> Decode.Slack.Event.event_of_json
      |> (
        ({eventType, Decode.Slack.Event.event}) =>
          switch (eventType) {
          | UrlVerification =>
            Decode.Slack.Event.(
              decode_verification(json)
              |> (
                ({challenge}) =>
                  Ezjsonm.(
                    Lwt.return(dict([("challenge", string(challenge))]))
                  )
              )
            )
          | EventCallback =>
            switch (event) {
            | Some(e) =>
              e.command |> Service.handle >>= Message.reply(`Slack(e))
            | _ =>
              Ezjsonm.(Lwt.return(dict([("challenge", string("hello"))])))
            }
          | UnknownEvent =>
            Ezjsonm.(Lwt.return(dict([("success", string("false"))])))
          }
      );

    `Json(result) |> respond';
  });

let command =
  post("/command", req => {
    let json = req.body |> Rock.Body.to_string |> Yojson.Basic.from_string;

    let%lwt result =
      json |> Api.decode |> Service.handle >>= Message.reply(`Api);

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
          Logs.set_level(Some(Logs.Debug));
          app;
        }
      );

    Lwt.async(() => s >>= (_ => Lwt.return_unit));
    Lwt_main.run(fst(Lwt.wait()));
  | `Error => exit(1)
  | `Not_running => exit(0)
  };
};
