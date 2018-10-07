type artist = {
  name: string
};

type item = {
  artists: array(artist),
  name: string,
  uri: string
};

type tracks = {
  items: array(item)
};

type data = {
  tracks: tracks
};

module Decode = {
  let artist = json => Json.Decode.{
    "name": json |> field("name", string),
  };

  let item = json => Json.Decode.{
    "artists": json |> field("artists", array(artist)),
    "name": json |> field("name", string),
    "uri": json |> field("uri", string)
  };

  let tracks = json => Json.Decode.{
    "items": json |> field("items", array(item))
  }

  let data = json => Json.Decode.{
    "tracks": json |> field("tracks", tracks)
  };
};

let searchTrack = (query: string, sendMessage) => {
  let headers = {"Authorization": "Bearer " ++ Config.spotifyToken};

  let url =
    "https://api.spotify.com/v1/search?q="
    ++ (query |> Js.String.split(" ") |> Js.Array.joinWith("%20"))
    ++ "&type=track&limit=5&market=SE";

  let request = Axios.makeConfigWithUrl(~url, ~_method="GET", ~headers, ());

  Js.Promise.(Axios.request(request) |> then_(posted => {
    let response = posted##data |> Decode.data;

    let attachments = response##tracks##items |> Array.map(item => {
      let artists = item##artists |> Array.map(artist => artist##name) |> Js.Array.joinWith(", ");
      
      ({
      "text": artists ++ " - " ++ item##name,
      "callback_id": "queue",
      "actions": [|
        {
                        "name": "track",
              "text": "Queue",
              "type": "button",
              "value": item##uri,
        }
      |] 
    })
    });

    sendMessage("Searching for *" ++ query ++ "*", attachments);

    posted |> resolve
  })) |> ignore;
};
