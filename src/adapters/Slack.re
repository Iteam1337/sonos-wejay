module Attachment = {
  [@bs.obj]
  external field: (~title: string, ~value: string, ~short: bool) => _ = "";

  [@bs.obj]
  external action:
    (
      ~name: [@bs.as "track"] _,
      ~text: [@bs.as "Queue"] _,
      ~_type: [@bs.as "button"] _,
      ~value: string,
      unit
    ) =>
    _ =
    "";

  [@bs.obj]
  external make:
    (
      ~color: [@bs.as "#efb560"] _,
      ~callback_id: [@bs.as "queue"] _,
      ~thumb_url: string,
      ~fields: array(Js.t('a)),
      ~actions: array(Js.t('b)),
      unit
    ) =>
    _ =
    "";
};

let userId = id => "<@" ++ id ++ ">";

let sendPayload = payload => {
  Js.Promise.(
    API.createRequest(
      ~url="https://slack.com/api/chat.postMessage",
      ~_method="POST",
      ~data=Some(payload),
      ~headers=Some({"Authorization": "Bearer " ++ Config.slackToken}),
      (),
    )
    |> then_(posted => posted |> resolve)
  )
  |> ignore;
};

let getUser = token => {
  Js.Promise.(
    API.createRequest(
      ~url="https://slack.com/api/auth.test",
      ~_method="POST",
      ~data=Some({"token": token}),
      ~headers=Some({"Authorization": "Bearer " ++ token}),
      (),
    )
    |> then_(user =>
         Belt.Option.getWithDefault(user##data##user_id, "") |> resolve
       )
  );
};

let makeAuthCallback = code =>
  API.createRequest(
    ~url=
      "https://slack.com/api/oauth.access?client_id="
      ++ Config.slackClientId
      ++ "&client_secret="
      ++ Config.slackClientSecret
      ++ "&code="
      ++ code,
    (),
  );

module Message = {
  [@bs.obj]
  external slackMessage:
    (
      ~channel: string,
      ~username: [@bs.as "Wejay"] _,
      ~text: string,
      ~attachments: array(Js.t('a))=?,
      ~mrkdwn: bool,
      unit
    ) =>
    _ =
    "";

  let withAttachments = (channel, message, attachments) =>
    slackMessage(~channel, ~text=message, ~attachments, ~mrkdwn=true, ())
    |> sendPayload;

  let regular = (channel: string, message: string) =>
    slackMessage(~channel, ~text=message, ~mrkdwn=true, ()) |> sendPayload;
};
