let handle =
  fun
  | Belt.Result.Ok(output) => output
  | Belt.Result.Error({Decco.path, message}) =>
    failwith({j|Decode error: $message ($path)|j});
