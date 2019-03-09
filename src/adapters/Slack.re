let sendPayload = payload => {
  let request =
    Axios.makeConfigWithUrl(
      ~url="https://slack.com/api/chat.postMessage",
      ~_method="POST",
      ~data=payload,
      ~headers={"Authorization": "Bearer " ++ Config.slackToken},
      (),
    );

  Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve))
  |> ignore;
};

let getUser = token => {
  let payload = {"token": token};

  let request =
    Axios.makeConfigWithUrl(
      ~url="https://slack.com/api/auth.test",
      ~_method="POST",
      ~data=payload,
      ~headers={"Authorization": "Bearer " ++ token},
      (),
    );

  Js.Promise.(
    Axios.request(request) |> then_(user => user##data##user_id |> resolve)
  );
};

let makeAuthCallback = code =>
  Axios.get(
    "https://slack.com/api/oauth.access?client_id="
    ++ Config.slackClientId
    ++ "&client_secret="
    ++ Config.slackClientSecret
    ++ "&code="
    ++ code,
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