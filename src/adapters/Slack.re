module Block = {
  [@bs.deriving {jsConverter: newType}]
  type text = {
    _type: string,
    text: string,
  };

  [@bs.deriving {jsConverter: newType}]
  type accessory = {
    _type: string,
    action_id: option(string),
    alt_text: option(string),
    image_url: option(string),
    text: option(abs_text),
    value: option(string),
  };

  [@bs.deriving jsConverter]
  type section = {
    _type: string,
    elements: option(array(option(abs_accessory))),
    fields: option(array(abs_text)),
    accessory: option(abs_accessory),
    text: option(abs_text),
  };

  let base =
      (
        ~_type="section",
        ~fields=None,
        ~text=None,
        ~accessory=None,
        ~elements=None,
        (),
      ) => {
    sectionToJs({_type, fields, text, accessory, elements});
  };

  let baseAccessory =
      (
        ~_type,
        ~image_url=None,
        ~alt_text=None,
        ~text=None,
        ~value=None,
        ~action_id=None,
        (),
      ) => {
    accessoryToJs({_type, image_url, alt_text, text, value, action_id});
  };

  module Text = {
    let make = (~text, ~_type="mrkdwn", ()) => {
      textToJs({_type, text});
    };
  };

  module Image = {
    let make = (~image_url, ~alt_text, ~_type="image", ()) => {
      Some(
        baseAccessory(
          ~_type,
          ~image_url=Some(image_url),
          ~alt_text=Some(alt_text),
          (),
        ),
      );
    };
  };

  module Button = {
    let make = (~text, ~value, ~action_id, ~_type="button", ()) => {
      Some(
        baseAccessory(
          ~_type,
          ~value=Some(value),
          ~action_id=Some(action_id),
          ~text=Some(Text.make(~text, ~_type="plain_text", ())),
          (),
        ),
      );
    };
  };

  module Divider = {
    let make = () => base(~_type="divider", ());
  };

  module Fields = {
    let make = (~fields, ~accessory=None, ()) =>
      base(~fields=Some(fields), ~accessory, ());
  };

  module Actions = {
    let make = (~elements) =>
      base(~_type="actions", ~elements=Some(elements), ());
  };

  module Section = {
    let make = (~text, ~_type="mrkdwn", ~accessory=None, ()) =>
      base(~accessory, ~text=Some(Text.make(~_type, ~text, ())), ());
  };

  module Simple = {
    let make = (~message as text) => {
      [|Section.make(~text, ())|];
    };
  };
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
      ~text: string=?,
      ~attachments: array(Js.t('a))=?,
      ~blocks: array(Js.t('a))=?,
      ~mrkdwn: bool=?,
      unit
    ) =>
    _ =
    "";

  let make = (channel, blocks) =>
    slackMessage(~channel, ~blocks, ()) |> sendPayload;
};
