open Belt.Result;

module Verification = {
  [@decco]
  type t = {
    challenge: string,
    token: string,
  };

  let make = json => t_decode(json)->Parser.handle;
};

module Requester = {
  type t =
    | Bot
    | Human(Commands.t);

  let make = (~subtype, ~command) => {
    switch (subtype) {
    | Some(t) =>
      switch (t) {
      | "bot_message" => Bot
      | _ => Human(command)
      }
    | None => Human(command)
    };
  };
};

module EventPayload = {
  [@decco]
  type t = {
    channel: string,
    text: option(string),
    subtype: option(string),
    user: option(string),
  };
};

module Event = {
  type t = {
    channel: string,
    command: Requester.t,
    text: string,
    user: option(string),
  };

  let parseText = text =>
    text
    |> Utils.removeUser
    |> Js.String.split(" ")
    |> Js.Array.sliceFrom(1)
    |> Js.Array.joinWith(" ");

  let make = (event: option(EventPayload.t)) => {
    switch (event) {
    | Some({channel, text, subtype, user}) =>
      Some({
        channel,
        command: Requester.make(~subtype, ~command=Commands.make(text)),
        text: text->Belt.Option.getWithDefault("")->parseText,
        user,
      })
    | None => None
    };
  };
};

module EventType = {
  type t =
    | UrlVerification
    | EventCallback(option(Event.t))
    | UnknownEvent;

  let make = (eventString, event) =>
    switch (eventString) {
    | "url_verification" => UrlVerification
    | "app_mention"
    | "event_callback" => EventCallback(Event.make(event))
    | _ => UnknownEvent
    };
};

module EventResponse = {
  [@decco]
  type payload = {
    [@decco.key "type"]
    type_: string,
    event: option(EventPayload.t),
  };

  type t = EventType.t;

  let make = json => {
    switch (payload_decode(json)) {
    | Ok({event, type_}) => EventType.make(type_, event)
    | Error({Decco.path, message}) => Parser.fail(message, path)
    };
  };
};

module Action = {
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
  type actionResponse = {payload: string};

  [@decco]
  type t = {
    actions,
    response_url: string,
    channel,
    user,
  };

  let make = json => {
    switch (actionResponse_decode(json)) {
    | Ok(output) => output.payload->Js.Json.parseExn->t_decode->Parser.handle
    | Error({Decco.path, message}) => Parser.fail(message, path)
    };
  };
};
