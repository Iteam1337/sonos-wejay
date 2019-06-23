module Slack = {
  module Event = {
    type requester =
      | Bot
      | Human;

    type eventType =
      | EventCallback
      | UrlVerification
      | UnknownEvent;

    let decode_command = text =>
      text
      |> String.split_on_char(' ')
      |> (
        words => (
          words |> List.hd |> Command.of_string,
          words |> List.tl |> String.concat(" "),
        )
      );

    let decode_event_type =
      fun
      | "event_callback" => EventCallback
      | "url_verification" => UrlVerification
      | _ => UnknownEvent;

    type verification = {challenge: string};

    let decode_verification = json =>
      Yojson.Safe.Util.{challenge: member("challenge", json) |> to_string};

    type event = {
      channel: string,
      subtype: requester,
      user: string,
      command: Api.t,
    };

    type t = {
      event: option(event),
      eventType,
    };

    let event_type_from_json = json =>
      Yojson.Safe.Util.(member("type", json) |> to_string)
      |> decode_event_type;

    let of_json = json => {
      let (command, args) =
        Yojson.Safe.Util.(member("text", json) |> to_string) |> decode_command;

      Yojson.Safe.Util.{
        channel: member("channel", json) |> to_string,
        command: {
          command,
          args,
        },
        subtype:
          member("subtype", json)
          |> to_string
          |> (
            fun
            | "bot_message" => Bot
            | _ => Human
          ),
        user: member("user", json) |> to_string,
      };
    };

    let event_of_json = json => {
      let eventJson =
        Yojson.Safe.Util.(
          member("event", json)
          |> (
            fun
            | `Null => None
            | v => Some(v)
          )
        );

      Yojson.Safe.Util.{
        eventType: json |> member("type") |> to_string |> decode_event_type,
        event:
          eventJson
          |> (
            fun
            | Some(e) => Some(e |> of_json)
            | _ => None
          ),
      };
    };
  };
};

module Spotify = {
  module Track = {
    type t = {
      albumName: string,
      artist: string,
      cover: string,
      duration: int,
      id: string,
      name: string,
      releaseDate: string,
      uri: string,
    };

    let of_json = json =>
      Ezjsonm.{
        albumName: get_string(find(json, ["albumName"])),
        artist: get_string(find(json, ["artist"])),
        cover: get_string(find(json, ["cover"])),
        duration: get_int(find(json, ["duration"])),
        id: get_string(find(json, ["id"])),
        name: get_string(find(json, ["name"])),
        releaseDate: get_string(find(json, ["releaseDate"])),
        uri: get_string(find(json, ["uri"])),
      };

    let to_json = track =>
      Ezjsonm.(
        dict([
          ("albumName", string(track.albumName)),
          ("artist", string(track.artist)),
          ("cover", string(track.cover)),
          ("duration", int(track.duration)),
          ("id", string(track.id)),
          ("name", string(track.name)),
          ("releaseDate", string(track.releaseDate)),
          ("uri", string(track.uri)),
        ])
      );
  };

  module Tracks = {
    type t = list(Track.t);

    let of_json = json =>
      Ezjsonm.(find(json, ["tracks"]) |> get_list(Track.of_json));

    let to_json = json => Ezjsonm.(json |> list(Track.to_json));
  };
};

/* module SpotifyYojson = { */
/*   module Track = { */
/*     type t = { */
/*       albumName: string, */
/*       artist: string, */
/*       cover: string, */
/*       duration: int, */
/*       id: string, */
/*       name: string, */
/*       releaseDate: string, */
/*       uri: string, */
/*     }; */

/*     let of_json = json => */
/*       Yojson.Safe.Util.{ */
/*         albumName: json |> member("albumName") |> to_string, */
/*         artist: json |> member("artist") |> to_string, */
/*         cover: json |> member("cover") |> to_string, */
/*         duration: json |> member("duration") |> to_int, */
/*         id: json |> member("id") |> to_string, */
/*         name: json |> member("name") |> to_string, */
/*         releaseDate: json |> member("releaseDate") |> to_string, */
/*         uri: json |> member("uri") |> to_string, */
/*       }; */

/*     let to_json = track => */
/*       `Assoc([ */
/*         ("albumName", `String(track.albumName)), */
/*         ("artist", `String(track.artist)), */
/*         ("cover", `String(track.cover)), */
/*         ("duration", `Int32(track.duration)), */
/*         ("id", `String(track.id)), */
/*         ("name", `String(track.name)), */
/*         ("releaseDate", `String(track.releaseDate)), */
/*         ("uri", `String(track.uri)), */
/*       ]); */
/*   }; */

/*   module Tracks = { */
/*     type t = list(Track.t); */

/*     let of_json = json => */
/*       Yojson.Safe.Util.( */
/*         json |> member("tracks") |> (tracks => tracks |> Track.of_json) */
/*       ); */

/*     let to_json = json => */
/*       Yojson.Safe.Util.(json |> (tracks => `List([Track.to_json(tracks)]))); */
/*   }; */
/* }; */
