[@bs.module "query-string"] external stringify: 'a => 'a = "";

type images = {url: string};
type album = {
  images: array(images),
  name: string,
};
type artist = {name: string};

type track = {
  album,
  artists: array(artist),
  duration: int,
  name: string,
  uri: string,
};

type tracks = {items: array(track)};

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

  let track = json => {
    album: json |> field("album", album),
    artists: json |> field("artists", array(artist)),
    duration: json |> field("duration_ms", int),
    name: json |> field("name", string),
    uri: json |> field("uri", string),
  };

  let tracks = json => {items: json |> field("items", array(track))};

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

let buildArtist = artists =>
  artists
  |> Array.map((artist: artist) => artist.name)
  |> Js.Array.joinWith(", ");

let displayTracks = item =>
  Utils.createAttachment(
    ~text=
      "*"
      ++ buildArtist(item.artists)
      ++ " - "
      ++ item.name
      ++ "*\n"
      ++ item.album.name,
    ~thumbUrl=item.album.images[0].url,
    ~uri=item.uri,
    (),
  );

let getTrack = (uri: string) =>
  Js.Promise.(
    getToken()
    |> then_(token => {
         let url = "https://api.spotify.com/v1/tracks/" ++ uri;

         Axios.makeConfigWithUrl(
           ~url,
           ~_method="GET",
           ~headers={"Authorization": "Bearer " ++ token.accessToken},
           (),
         )
         |> Axios.request
         |> then_(posted => {
              let track = posted##data |> Decode.track;

              track |> resolve;
            })
         |> catch(Utils.handleError("spotifyGetTrack"));
       })
  );

let searchTrack = (query: string) =>
  Js.Promise.(
    getToken()
    |> then_(token => {
         let url = SpotifyUtils.spotifySearchUrl(~query, ());

         Axios.makeConfigWithUrl(
           ~url,
           ~_method="GET",
           ~headers={"Authorization": "Bearer " ++ token.accessToken},
           (),
         )
         |> Axios.request
         |> then_(posted => {
              let {tracks} = posted##data |> Decode.data;

              tracks |> resolve;
            })
         |> catch(Utils.handleError("spotifySearchTrack"));
       })
  );

let searchTrackWithMessage = (query: string, sendMessageWithAttachments) =>
  Js.Promise.(
    searchTrack(query)
    |> then_(tracks => {
         let message =
           switch (Belt.Array.length(tracks.items)) {
           | 0 => "Sorry, I couldn't find anything with *" ++ query ++ "*"
           | _ => "Here are the results for *" ++ query ++ "*"
           };

         tracks.items->Belt.Array.map(displayTracks)
         |> sendMessageWithAttachments(message);

         tracks |> resolve;
       })
    |> catch(Utils.handleError("spotifySearchTrackWithMessage"))
  )
  |> ignore;
