[@bs.scope "JSON"] [@bs.val]
external parseToJson : string => Js.Json.t = "parse";

type command =
  | Clear
  | CurrentQueue
  | CurrentTrack
  | Mute
  | Next
  | Pause
  | Play
  | Previous
  | Queue
  | Search
  | Unknown
  | Unmute
  | Volume;

type eventType =
  | UrlVerification
  | EventCallback
  | UnknownEvent;

type messageType = {eventType};

type verification = {
  challenge: string,
  token: string,
};

type event = {
  command,
  channel: string,
  text: string,
  subtype: string,
  user: string,
};

type message = {event};

type action = {value: string};
type actions = array(action);

let type_ = json =>
  Json.Decode.{
    "eventType":
      switch (json |> field("type", string)) {
      | "url_verification" => UrlVerification
      | "event_callback" => EventCallback
      | _ => UnknownEvent
      },
  };

let verification = json =>
  Json.Decode.{
    "challenge": json |> field("challenge", string),
    "token": json |> field("token", string),
  };

let event = json =>
  Json.Decode.{
    "channel": json |> field("channel", string),
    "command":
      switch (json |> optional(field("text", string))) {
      | Some(text) =>
        switch (
          text
          |> Js.String.toLowerCase
          |> Js.String.split(" ")
          |> Js.Array.slice(~start=0, ~end_=1)
        ) {
        | [|"clear"|] => Clear
        | [|"current"|]
        | [|"np"|]
        | [|"nowplaying"|] => CurrentTrack
        | [|"currentqueue"|]
        | [|"getqueue"|] => CurrentQueue
        | [|"mute"|] => Mute
        | [|"next"|] => Next
        | [|"pause"|] => Pause
        | [|"play"|] => Play
        | [|"previous"|] => Previous
        | [|"q"|]
        | [|"queue"|] => Queue
        | [|"s"|]
        | [|"search"|] => Search
        | [|"unmute"|] => Unmute
        | [|"volume"|] => Volume
        | _ => Unknown
        }
      | None => Unknown
      },
    "subtype": json |> optional(field("subtype", string)),
    "text":
      switch (json |> optional(field("text", string))) {
      | Some(text) =>
        text
        |> Js.String.split(" ")
        |> Js.Array.sliceFrom(1)
        |> Js.Array.joinWith(" ")
      | None => ""
      },
    "user": json |> optional(field("user", string)),
  };

let message = json => Json.Decode.{"event": json |> field("event", event)};

let action = json => Json.Decode.{
  "value": json |> field("value", string)
};

let channel = json => Json.Decode.{
  "id": json |> field("id", string),
};

let actionPayload = json => Json.Decode.{
  "actions": json |> field("actions", array(action)),
  "channel": json |> field("channel", channel),

}

let parseAction = json => Json.Decode.{
  "payload": json |> field("payload", string) |> parseToJson
};
