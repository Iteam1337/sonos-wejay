[@bs.module "@wejay/spotify"]
external _getTrack: string => Js.Promise.t(Js.Json.t) = "getTrack";

[@bs.module "@wejay/spotify"]
external _search: string => Js.Promise.t(Js.Json.t) = "search";

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

  type tracks = {tracks: array(t)};

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

  let tracks = json => json |> Json.Decode.array(decode);
};

let getSpotifyTrack = id => {
  Js.Promise.(
    _getTrack(id)
    |> then_(response => response |> WejayTrack.decode |> resolve)
  );
};

let createSearchAttachment =
    ({albumName, artist, cover, duration, name, uri}: WejayTrack.t) => {
  Slack.Attachment.(
    make(
      ~thumb_url=cover,
      ~fields=[|
        field(~title="Artist", ~value=artist, ~short=true),
        field(~title="Title", ~value=name, ~short=true),
        field(~title="Album", ~value=albumName, ~short=true),
        field(
          ~title="Duration",
          ~value=Utils.parseDuration(duration /. 1000.),
          ~short=true,
        ),
      |],
      ~actions=[|action(~value=uri, ())|],
      (),
    )
  );
};

let search = query => {
  Js.Promise.(
    _search(query |> Js.Global.encodeURIComponent)
    |> then_(response => {
         let tracks = response |> WejayTrack.tracks;

         let message =
           switch (Belt.Array.length(tracks)) {
           | 0 => {j|Sorry, I couldn't find anything with *$query*|j}
           | _ => {j|Here are the results for *$query*|j}
           };

         let attachments =
           tracks
           ->Belt.Array.slice(~offset=0, ~len=5)
           ->Belt.Array.map(createSearchAttachment);

         resolve(`Ok((message, attachments)));
       })
  );
};
