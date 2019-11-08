open Express;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.get(app, ~path="/") @@ Routes.IndexRoute.make;

/* Slack events */
App.post(app, ~path="/event") @@ Routes.EventRoute.make;
App.post(app, ~path="/action") @@ Routes.ActionRoute.make;

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ =>
    Js.log(
      {j|
Wejay is up and running
-----------------------------
Server: http://localhost:3000
  |j},
    )
  };

App.listen(app, ~port=3000, ~onListen, ());
