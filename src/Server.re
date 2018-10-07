open Express;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.post(app, ~path="/event") @@ Routes.event;

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log("Listening at http://localhost:3000")
  };

App.listen(app, ~port=3000, ~onListen, ());
