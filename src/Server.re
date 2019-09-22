open Express;

let app = express();
let device = Sonos.Methods.device(Config.wejayIp);

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.get(app, ~path="/") @@ Routes.IndexRoute.make;

/* Slack authentication */
App.get(app, ~path="/slack/auth") @@ Routes.SlackRoutes.auth;
App.get(app, ~path="/slack/token") @@ Routes.SlackRoutes.token;

/* Slack events */
App.post(app, ~path="/event") @@ Routes.EventRoute.make;
App.post(app, ~path="/action") @@ Routes.ActionRoute.make;

/* CLI */
App.post(app, ~path="/cli") @@ CLI.route;

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log("Listening at http://localhost:3000")
  };

App.listen(app, ~port=3000, ~onListen, ());
