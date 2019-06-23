Sys.getenv_opt("WEJAY_IP")
|> (
  fun
  | Some(e) => print_endline(e)
  | _ => print_endline("Not found")
);

Wejay.Server.run();
