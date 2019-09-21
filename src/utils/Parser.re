open Belt.Result;

let fail = (msg, path) => failwith({j|Decode error: $msg ($path)|j});

let handle =
  fun
  | Ok(output) => output
  | Error({Decco.path, message}) => fail(message, path);
