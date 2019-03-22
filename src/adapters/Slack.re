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

let sendSearchResponse = (channel, message, attachments) =>
  {
    "channel": channel,
    "username": "Wejay",
    "text": message,
    "attachments": attachments,
    "mrkdwn": true,
  }
  |> sendPayload;

let sendResponseWithAttachments =
    (channel: string, message: string, attachments) =>
  {
    "channel": channel,
    "username": "Wejay",
    "text": message,
    "attachments": attachments,
    "mrkdwn": true,
  }
  |> sendPayload;

let sendSlackResponse = (channel: string, message: string) =>
  {"channel": channel, "username": "Wejay", "text": message, "mrkdwn": true}
  |> sendPayload;

let userId = id => "<@" ++ id ++ ">";
