open Express;

let app = express();
let device = Sonos.Methods.device(Config.wejayIp);

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.get(app, ~path="/") @@ Routes.index;
App.post(app, ~path="/event") @@ Routes.event;
App.post(app, ~path="/action") @@ Routes.action;
App.post(app, ~path="/cli") @@ CLI.route;
App.get(app, ~path="/slack/auth") @@ Routes.slackAuth;
App.get(app, ~path="/slack/token") @@ Routes.slackToken;

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log("Listening at http://localhost:3000")
  };

App.listen(app, ~port=3000, ~onListen, ());