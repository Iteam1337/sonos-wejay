[@bs.module "query-string"] external stringify: 'a => 'a = "";

type images = {url: string};
type album = {
  images: array(images),
  name: string,
};
type artist = {name: string};

type item = {
  album,
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
  open Json.Decode;

  let images = json => {url: json |> field("url", string)};

  let album = json => {
    images: json |> field("images", array(images)),
    name: json |> field("name", string),
  };

  let artist = json => {name: json |> field("name", string)};

  let item = json => {
    album: json |> field("album", album),
    artists: json |> field("artists", array(artist)),
    name: json |> field("name", string),
    uri: json |> field("uri", string),
  };

  let tracks = json => {items: json |> field("items", array(item))};

  let data = json => {tracks: json |> field("tracks", tracks)};

  let token = json => {
    accessToken: json |> field("access_token", string),
    tokenType: json |> field("token_type", string),
    expiresIn: json |> field("expires_in", int),
    scope: json |> field("scope", string),
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
    |> catch(Utils.handleError("spotifyToken"))
  );

let displayTracks = item => {
  let artists =
    item.artists
    |> Array.map((artist: artist) => artist.name)
    |> Js.Array.joinWith(", ");

  Utils.createAttachment(
    ~text="*" ++ artists ++ " - " ++ item.name ++ "*\n" ++ item.album.name,
    ~thumbUrl=item.album.images[0].url,
    ~uri=item.uri,
    (),
  );
};

let searchTrack = (query: string, sendMessageWithAttachments) =>
  Js.Promise.(
    getToken()
    |> then_(token => {
         let url =
           "https://api.spotify.com/v1/search?q="
           ++ (query |> Js.Global.encodeURIComponent)
           ++ "&type=track&limit=5&market=SE";

         Axios.makeConfigWithUrl(
           ~url,
           ~_method="GET",
           ~headers={"Authorization": "Bearer " ++ token.accessToken},
           (),
         )
         |> Axios.request
         |> then_(posted => {
              let {tracks} = posted##data |> Decode.data;

              let message =
                switch (Belt.Array.length(tracks.items)) {
                | 0 => "Sorry, I couldn't find anything with *" ++ query ++ "*"
                | _ => "Here are the results for *" ++ query ++ "*"
                };

              tracks.items->Belt.Array.map(displayTracks)
              |> sendMessageWithAttachments(message);

              posted |> resolve;
            })
         |> catch(Utils.handleError("spotifySearchTrack"));
       })
  )
  |> ignore;
