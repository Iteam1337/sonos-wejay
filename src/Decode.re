[@bs.scope "JSON"] [@bs.val]
external parseToJson: string => Js.Json.t = "parse";

type egg =
  | FreeBird
  | Friday
  | Shoreline
  | Tequila;

type command =
  | Clear
  | CurrentQueue
  | EasterEgg(egg)
  | Mute
  | Next
  | NowPlaying
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

let decodeCommand = text =>
  switch (
    text
    |> Js.String.toLowerCase
    |> Js.String.split(" ")
    |> Js.Array.slice(~start=0, ~end_=1)
    |> (array => array[0])
  ) {
  | "clear" => Clear
  | "current"
  | "freebird" => EasterEgg(FreeBird)
  | "friday" => EasterEgg(Friday)
  | "np"
  | "nowplaying" => NowPlaying
  | "currentqueue"
  | "getqueue" => CurrentQueue
  | "mute" => Mute
  | "next" => Next
  | "pause" => Pause
  | "play" => Play
  | "previous" => Previous
  | "q"
  | "queue" => Queue
  | "search" => Search
  | "shoreline" => EasterEgg(Shoreline)
  | "tequila" => EasterEgg(Tequila)
  | "unmute" => Unmute
  | "volume" => Volume
  | _ => Unknown
  };

let event = json =>
  Json.Decode.{
    "channel": json |> field("channel", string),
    "command":
      switch (json |> optional(field("text", string))) {
      | Some(text) => text |> decodeCommand
      | None => Unknown
      },
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

let action = json => Json.Decode.{"value": json |> field("value", string)};

let channel = json => Json.Decode.{"id": json |> field("id", string)};

let actionPayload = json =>
  Json.Decode.{
    "actions": json |> field("actions", array(action)),
    "channel": json |> field("channel", channel),
  };

let parseAction = json =>
  Json.Decode.{"payload": json |> field("payload", string) |> parseToJson};
