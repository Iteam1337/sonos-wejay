let sendPayload = payload => {
  let request =
    Axios.makeConfigWithUrl(
      ~url="https://slack.com/api/chat.postMessage",
      ~_method="POST",
      ~data=payload,
      ~headers={"Authorization": "Bearer " ++ Config.slackToken},
      (),
    );

  Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve));
};

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
