let createRequest = (~url, ~_method="GET", ~data=None, ()) => {
  switch (data) {
  | Some(d) =>
    Axios.makeConfigWithUrl(~url, ~_method, ~data=d, ()) |> Axios.request
  | None => Axios.makeConfigWithUrl(~url, ~_method, ()) |> Axios.request
  };
};
