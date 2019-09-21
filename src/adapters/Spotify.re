[@bs.module "@wejay/spotify"]
external _getTrack: string => Js.Promise.t(Js.Json.t) = "getTrack";

[@bs.module "@wejay/spotify"]
external _search: string => Js.Promise.t(Js.Json.t) = "search";

module WejayTrack = {
  [@decco]
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

  [@decco]
  type tracks = array(t);

  let tracks = json => tracks_decode(json)->Parser.handle;
  let track = json => t_decode(json)->Parser.handle;
};

let getSpotifyTrack = id => {
  Js.Promise.(
    _getTrack(id)
    |> then_(response => response |> WejayTrack.track |> resolve)
  );
};

let createSearchAttachment =
    ({albumName, artist, cover, duration, name, uri}: WejayTrack.t) => {
  let trackDuration = Utils.parseDuration(duration /. 1000.0);

  Slack.Block.(
    [|
      Divider.make(),
      Fields.make(
        ~accessory=Image.make(~image_url=cover, ~alt_text="Album cover", ()),
        ~fields=[|
          Text.make(~text={j|*Artist*\n$artist|j}, ()),
          Text.make(~text={j|*Track name*\n$name|j}, ()),
          Text.make(~text={j|*Album*\n$albumName|j}, ()),
          Text.make(~text={j|*Current position*\n$trackDuration|j}, ()),
        |],
        (),
      ),
      Actions.make(
        ~elements=[|
          Button.make(
            ~text="Queue track",
            ~value=uri,
            ~action_id="queue_new_track",
            (),
          ),
        |],
      ),
    |]
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
           ->Belt.Array.reduce([||], (acc, curr) =>
               acc->Belt.Array.concat(createSearchAttachment(curr))
             );

         resolve(
           `Ok((
             message,
             [|
               Slack.Block.Section.make(
                 ~text={j|Here are the results for *$query*|j},
                 (),
               ),
             |]
             ->Belt.Array.concat(attachments),
           )),
         );
       })
  );
};
