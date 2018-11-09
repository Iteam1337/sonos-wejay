let let_ = (promise, continuation) =>
  promise
  |> Js.Promise.then_(value => continuation(value) |> Js.Promise.resolve);
