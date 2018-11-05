[@bs.scope "JSON"] [@bs.val]
external parseToJson: string => Js.Json.t = "parse";

type subType =
  | Bot
  | Human;

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
  command: Commands.command,
  channel: string,
  subtype: subType,
  text: string,
  user: option(string),
};

type message = {event};

type action = {value: string};
type actions = array(action);

type channel = {id: string};

type user = {
  id: string,
  name: string,
};

type actionPayload = {
  actions,
  channel,
  user,
};

let type_ = json =>
  Json.Decode.{
    eventType:
      switch (json |> field("type", string)) {
      | "url_verification" => UrlVerification
      | "app_mention"
      | "event_callback" => EventCallback
      | _ => UnknownEvent
      },
  };

let verification = json =>
  Json.Decode.{
    challenge: json |> field("challenge", string),
    token: json |> field("token", string),
  };

let parseQuery = text =>
  text
  |> Utils.removeUser
  |> Js.String.split(" ")
  |> Js.Array.sliceFrom(1)
  |> Js.Array.joinWith(" ");

let event = json =>
  Json.Decode.{
    channel: json |> field("channel", string),
    command: json |> field("text", string) |> Commands.decodeCommand,
    text:
      switch (json |> optional(field("text", string))) {
      | Some(text) => parseQuery(text)
      | None => ""
      },
    subtype:
      switch (json |> optional(field("subtype", string))) {
      | Some(subtype) =>
        switch (subtype) {
        | "bot_message" => Bot
        | _ => Human
        }
      | None => Human
      },
    user: json |> optional(field("user", string)),
  };

let message = json => Json.Decode.{event: json |> field("event", event)};

let action = json => Json.Decode.{value: json |> field("value", string)};

let channel = json => Json.Decode.{id: json |> field("id", string)};

let user = json =>
  Json.Decode.{
    id: json |> field("id", string),
    name: json |> field("name", string),
  };

let actionPayload = json =>
  Json.Decode.{
    actions: json |> field("actions", array(action)),
    channel: json |> field("channel", channel),
    user: json |> field("user", user),
  };

let parseAction = json =>
  Json.Decode.{"payload": json |> field("payload", string) |> parseToJson};
