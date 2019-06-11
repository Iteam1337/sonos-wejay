module Requester = {
  type t =
    | Bot
    | Human;
};

type eventType =
  | UrlVerification
  | EventCallback
  | UnknownEvent;

type verification = {
  challenge: string,
  token: string,
};

type event = {
  command: Commands.t,
  channel: string,
  subtype: Requester.t,
  text: string,
  user: option(string),
};

type eventPayload = {
  event: option(event),
  eventType,
};

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
    command:
      Belt.Option.getWithDefault(
        json |> optional(field("text", string)),
        "",
      )
      |> Commands.decodeCommand,
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

let eventPayload = json =>
  Json.Decode.{
    eventType:
      switch (json |> field("type", string)) {
      | "url_verification" => UrlVerification
      | "app_mention"
      | "event_callback" => EventCallback
      | _ => UnknownEvent
      },
    event: json |> optional(field("event", event)),
  };

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
  Json.Decode.{
    "payload": json |> field("payload", string) |> Js.Json.parseExn,
  };
