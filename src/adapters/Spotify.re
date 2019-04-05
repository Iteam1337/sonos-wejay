[@bs.module "query-string"] external stringify: 'a => 'a = "";

[@bs.module "@wejay/spotify"]
external _getTrack: string => Js.Promise.t(Js.Json.t) = "getTrack";

module Album = {
  type images = {url: string};
  type t = {
    images: array(images),
    name: string,
    releaseDate: string,
  };

  let images = json => Json.Decode.{url: json |> field("url", string)};

  let decode = json =>
    Json.Decode.{
      images: json |> field("images", array(images)),
      name: json |> field("name", string),
      releaseDate: json |> field("release_date", string),
    };
};

module Artist = {
  type t = {name: string};

  let decode = json => Json.Decode.{name: json |> field("name", string)};
};

module Track = {
  type t = {
    album: Album.t,
    artists: array(Artist.t),
    duration: int,
    name: string,
    uri: string,
  };

  type tracks = {items: array(t)};

  let track = json =>
    Json.Decode.{
      album: json |> field("album", Album.decode),
      artists: json |> field("artists", array(Artist.decode)),
      duration: json |> field("duration_ms", int),
      name: json |> field("name", string),
      uri: json |> field("uri", string),
    };

  let decode = json =>
    Json.Decode.{items: json |> field("items", array(track))};
};

module WejayTrack = {
  type t = {
    albumName: string,
    artist: string,
    cover: string,
    duration: float,
    id: string,
    name: string,
    releaseDate: string,
    uri: string,
  };

  let decode = json => {
    Json.Decode.{
      albumName: json |> field("albumName", string),
      artist: json |> field("artist", string),
      cover: json |> field("cover", string),
      duration: json |> field("duration", Json.Decode.float),
      id: json |> field("id", string),
      name: json |> field("name", string),
      releaseDate: json |> field("releaseDate", string),
      uri: json |> field("uri", string),
    };
  };
};

type data = {tracks: Track.tracks};

type token = {
  accessToken: string,
  tokenType: string,
  expiresIn: int,
  scope: string,
};

module Decode = {
  open Json.Decode;

  let data = json => {tracks: json |> field("tracks", Track.decode)};

  let token = json => {
    accessToken: json |> field("access_token", string),
    tokenType: json |> field("token_type", string),
    expiresIn: json |> field("expires_in", int),
    scope: json |> field("scope", string),
  };
};

let getToken = () =>
  Js.Promise.(
    API.createRequest(
      ~url="https://accounts.spotify.com/api/token",
      ~_method="POST",
      ~data=Some(stringify({"grant_type": "client_credentials"})),
      ~headers=Some({"Authorization": "Basic " ++ Config.spotifyAuth}),
      (),
    )
    |> then_(value => value##data |> Decode.token |> resolve)
    |> catch(Utils.handleError("spotifyToken"))
  );

let buildArtist = artists =>
  artists
  |> Array.map((artist: Artist.t) => artist.name)
  |> Js.Array.joinWith(", ");

let spotifyRequest = url =>
  Js.Promise.(
    getToken()
    |> then_(token =>
         API.createRequest(
           ~url,
           ~headers=Some({"Authorization": "Bearer " ++ token.accessToken}),
           (),
         )
         |> then_(response => resolve(response))
       )
  );

let getSpotifyTrack = id => {
  Js.Promise.(
    _getTrack(id)
    |> then_(response => response |> WejayTrack.decode |> resolve)
  );
};

let spotifySearch = query => {
  let url = SpotifyUtils.spotifySearchUrl(~query, ());

  Js.Promise.(
    spotifyRequest(url)
    |> then_(posted => {
         let {tracks} = posted##data |> Decode.data;
         resolve(tracks);
       })
    |> catch(Utils.handleError("spotifySearchTrack"))
  );
};

let attachmentField = (~title, ~value, ~short=true, ()) => {
  {"title": title, "value": value, "short": short};
};

let createAttachment = ({album, artists, duration, name, uri}: Track.t) => {
  "color": "#efb560",
  "callback_id": "queue",
  "thumb_url": album.images[0].url,
  "fields": [|
    attachmentField(~title="Artist", ~value=buildArtist(artists), ()),
    attachmentField(~title="Title", ~value=name, ()),
    attachmentField(~title="Album", ~value=album.name, ()),
    attachmentField(
      ~title="Duration",
      ~value=Utils.parseDuration(float_of_int(duration / 1000)),
      (),
    ),
  |],
  "actions": [|
    {"name": "track", "text": "Queue", "type": "button", "value": uri},
  |],
};

let searchTrack = (query, sendMessageWithAttachments) =>
  Js.Promise.(
    spotifySearch(query)
    |> then_((tracks: Track.tracks) => {
         let message =
           switch (Belt.Array.length(tracks.items)) {
           | 0 => {j|Sorry, I couldn't find anything with *$query*|j}
           | _ => {j|Here are the results for *$query*|j}
           };

         tracks.items->Belt.Array.map(createAttachment)
         |> sendMessageWithAttachments(message);

         tracks |> resolve;
       })
    |> catch(Utils.handleError("spotifySearchTrackWithMessage"))
  )
  |> ignore;
