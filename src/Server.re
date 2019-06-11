open Opium.Std;

let command = (req: Request.t) => {
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
};

let wejay = (_req: Request.t) => `Html("<h1>This is Wejay</h1>") |> respond';

let _ =
  App.empty
  |> post("/command", command)
  |> get("/", wejay)
  |> App.run_command;
