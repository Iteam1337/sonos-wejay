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
      Ezjsonm.{challenge: find(json, ["challenge"]) |> get_string};

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
      Ezjsonm.(find(json, ["type"]) |> get_string) |> decode_event_type;

    let of_json = json => {
      let (command, args) =
        Ezjsonm.(find(json, ["text"]) |> get_string) |> decode_command;

      Ezjsonm.{
        channel: find(json, ["channel"]) |> get_string,
        command: {
          command,
          args,
        },
        subtype:
          find(json, ["type"])
          |> get_string
          |> (
            fun
            | "bot_message" => Bot
            | _ => Human
          ),
        user: find(json, ["user"]) |> get_string,
      };
    };

    let event_of_json = json => {
      let eventJson =
        Ezjsonm.(
          switch (find(json, ["event"])) {
          | exception Not_found => None
          | v => Some(v)
          }
        );

      Ezjsonm.{
        eventType: find(json, ["type"]) |> get_string |> decode_event_type,
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
