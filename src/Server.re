open Express;

let app = express();

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

let listening = () => {
  let title = Chalk.hex("#F9A231")->Chalk.bold("Wejay");
  let url = Chalk.green("Server:");

  let value = {j|
$title is up and running
-----------------------------
$url http://localhost:3000
  |j};

  Boxen.make(
    ~value,
    ~padding=Some(1),
    ~margin=Some(1),
    ~borderStyle=`doubleSingle,
    ~borderColor=Some("blue"),
    (),
  );
};

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log(listening())
  };

App.listen(app, ~port=3000, ~onListen, ());
