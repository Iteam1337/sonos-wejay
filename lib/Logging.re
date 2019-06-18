open Opium.Std;
open Lwt.Infix;

/* This is done to demonstrate a usecase where the log reporter
   is returned via a Lwt promise */
let log_reporter = () => Lwt.return(Logs_fmt.reporter());

let logger = {
  let filter = (handler, req) => {
    let uri = Request.uri(req) |> Uri.path_and_query;
    handler(req)
    >|= (
      response => {
        let code = response |> Response.code |> Httpaf.Status.to_code;
        Logs.info(m => m("Responded to '%s' with %d", uri, code));
        response;
      }
    );
  };

  Rock.Middleware.create(~name="Logger", ~filter);
};
