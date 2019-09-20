module Requester = {
  type t =
    | Bot
    | Human;
};

type eventType =
  | UrlVerification
  | EventCallback
  | UnknownEvent;

[@decco]
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

[@decco]
type action = {value: string};

[@decco]
type actions = array(action);

[@decco]
type channel = {id: string};

[@decco]
type user = {
  id: string,
  name: string,
};

[@decco]
type actionPayload = {
  actions,
  channel,
  user,
};

[@decco]
type actionResponse = {payload: string};

let verification = json => verification_decode(json)->Parser.handle;

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

let parseAction = json => {
  switch (actionResponse_decode(json)) {
  | Belt.Result.Ok(output) =>
    output.payload->Js.Json.parseExn->actionPayload_decode->Parser.handle
  | Belt.Result.Error({Decco.path, message}) =>
    failwith({j|Decode error: $message ($path)|j})
  };
};
