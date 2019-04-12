let createRequest = (~url, ~_method="GET", ~data=None, ~headers=None, ()) => {
  let headers =
    switch (headers) {
    | Some(h) => Axios.Headers.fromObj(h)
    | None => Axios.Headers.fromDict(Js.Dict.empty())
    };

  let base = Axios.makeConfigWithUrl(~url, ~_method, ~headers);

  (
    switch (data) {
    | Some(d) => base(~data=d, ())
    | None => base()
    }
  )
  |> Axios.request;
};
