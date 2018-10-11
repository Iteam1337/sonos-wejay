[@bs.module "query-string"] external stringify: 'a => 'a = "";

type artist = {name: string};

type item = {
  artists: array(artist),
  name: string,
  uri: string,
};

type tracks = {items: array(item)};

type data = {tracks};

type token = {
  accessToken: string,
  tokenType: string,
  expiresIn: int,
  scope: string,
};

module Decode = {
  let images = json => Json.Decode.{"url": json |> field("url", string)};

  let album = json =>
    Json.Decode.{
      "images": json |> field("images", array(images)),
      "name": json |> field("name", string),
    };

  let artist = json => Json.Decode.{"name": json |> field("name", string)};

  let item = json =>
    Json.Decode.{
      "album": json |> field("album", album),
      "artists": json |> field("artists", array(artist)),
      "name": json |> field("name", string),
      "uri": json |> field("uri", string),
    };

  let tracks = json =>
    Json.Decode.{"items": json |> field("items", array(item))};

  let data = json => Json.Decode.{"tracks": json |> field("tracks", tracks)};

  let token = json =>
    Json.Decode.{
      "accessToken": json |> field("access_token", string),
      "tokenType": json |> field("token_type", string),
      "expiresIn": json |> field("expires_in", int),
      "scope": json |> field("scope", string),
    };
};

let getToken = () =>
  Js.Promise.(
    Axios.makeConfigWithUrl(
      ~url="https://accounts.spotify.com/api/token",
      ~_method="POST",
      ~data=stringify({"grant_type": "client_credentials"}),
      ~headers={"Authorization": "Basic " ++ Config.spotifyAuth},
      (),
    )
    |> Axios.request
    |> then_(value => value##data |> Decode.token |> resolve)
  );

let displayTracks = item => {
  let artists =
    item##artists
    |> Array.map(artist => artist##name)
    |> Js.Array.joinWith(", ");

  {
    "text": "*" ++ artists ++ " - " ++ item##name ++ "*\n" ++ item##album##name,
    "callback_id": "queue",
    "thumb_url": item##album##images[0]##url,
    "actions": [|
      {
        "name": "track",
        "text": "Queue",
        "type": "button",
        "value": item##uri,
      },
    |],
  };
};

let searchTrack = (query: string, sendMessage) =>
  Js.Promise.(
    getToken()
    |> then_(token => {
         let url =
           "https://api.spotify.com/v1/search?q="
           ++ (query |> Js.String.split(" ") |> Js.Array.joinWith("%20"))
           ++ "&type=track&limit=5&market=SE";

         Axios.makeConfigWithUrl(
           ~url,
           ~_method="GET",
           ~headers={"Authorization": "Bearer " ++ token##accessToken},
           (),
         )
         |> Axios.request
         |> then_(posted => {
              let response = posted##data |> Decode.data;

              response##tracks##items
              |> Array.map(displayTracks)
              |> sendMessage("Searching for *" ++ query ++ "*");

              posted |> resolve;
            });
       })
  )
  |> ignore;
