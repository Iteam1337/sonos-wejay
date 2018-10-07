type command =
  | Clear
  | CurrentQueue
  | Pause
  | Play
  | Queue
  | Search
  | Unknown;

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
        | [|"currentqueue"|]
        | [|"getqueue"|] => CurrentQueue
        | [|"pause"|] => Pause
        | [|"play"|] => Play
        | [|"queue"|] => Queue
        | [|"search"|] => Search
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
