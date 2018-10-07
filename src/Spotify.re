let searchTrack = (query: string) => {
  let headers = {"Authorization": "Bearer " ++ Config.spotifyToken};

  let url =
    "https://api.spotify.com/v1/search?q="
    ++ (query |> Js.String.split(" ") |> Js.Array.joinWith("%20"))
    ++ "&type=track&limit=5&market=SE";

  let request = Axios.makeConfigWithUrl(~url, ~_method="GET", ~headers, ());

  Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve));
};
