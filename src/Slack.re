
  let headers = {"Authorization": "Bearer " ++ Config.slackToken};

let sendResponseWithAttachments = (channel: string, message: string, attachments) => {
let payload = {
    "channel": channel,
    "username": "Wejay",
    "text": message,
    "attachments": attachments,
    "mrkdwn": true,
  };


  let request =
    Axios.makeConfigWithUrl(
      ~url="https://slack.com/api/chat.postMessage",
      ~_method="POST",
      ~data=payload,
      ~headers,
      (),
    );

  Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve))
  |> ignore;
}

let sendSlackResponse = (channel: string, message: string) => {
  let payload = {
    "channel": channel,
    "username": "Wejay",
    "text": message,
    "mrkdwn": true,
  };

  let request =
    Axios.makeConfigWithUrl(
      ~url="https://slack.com/api/chat.postMessage",
      ~_method="POST",
      ~data=payload,
      ~headers,
      (),
    );

  Js.Promise.(Axios.request(request) |> then_(posted => posted |> resolve))
  |> ignore;
};
