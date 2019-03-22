let createRequest = (~url, ~_method="GET", ~data=None, ~headers=None, ()) => {
  let base = Axios.makeConfigWithUrl(~url, ~_method);

  (
    switch (data, headers) {
    | (Some(d), Some(h)) => base(~data=d, ~headers=h, ())
    | (None, Some(h)) => base(~headers=h, ())
    | (Some(d), None) => base(~data=d, ())
    | (None, None) => base()
    }
  )
  |> Axios.request;
};
