module Result = {
  type t('a) =
    | Message(array(Js.t('a)));

  type result('a) = Belt.Result.t(t('a), string);
};

module Block = {
  module Text = {
    let make = (~text) => {"type": "mrkdwn", "text": text} |> Obj.magic;
  };

  module Divider = {
    let make = () => {"type": "divider"} |> Obj.magic;
  };

  module Section = {
    let make = (~text) =>
      {"type": "section", "text": Text.make(~text)} |> Obj.magic;
  };

  module Accessory = {
    type t = [ | `Image(string, string)];

    module Button = {
      type t = {
        text: string,
        value: string,
        action_id: string,
      };

      let make = (~text, ~value, ~action_id) =>
        {
          "type": "button",
          "action_id": action_id,
          "text": {
            "type": "plain_text",
            "text": text,
          },
          "value": value,
        }
        |> Obj.magic;
    };

    module Image = {
      let make = (~image_url, ~alt_text) =>
        {"type": "image", "image_url": image_url, "alt_text": alt_text}
        |> Obj.magic;
    };
  };

  module Actions = {
    type t = [ | `Button(Accessory.Button.t)];

    let make = (~elements) => {
      let elements =
        elements
        ->Belt.List.map(element =>
            switch (element) {
            | `Button(({text, value, action_id}: Accessory.Button.t)) =>
              Accessory.Button.make(~text, ~value, ~action_id)
            }
          )
        ->Belt.List.toArray;

      {"type": "actions", "elements": elements} |> Obj.magic;
    };
  };

  module Field = {
    type t = [ | `Text(string)];

    let handleAccessory =
      fun
      | `Image(image_url, alt_text) =>
        Accessory.Image.make(~image_url, ~alt_text);

    let handleFields =
      fun
      | `Text(text) => Text.make(~text);

    module WithImage = {
      let make = (~fields, ~accessory) => {
        let accessory = handleAccessory(accessory);
        let fields = fields->Belt.List.map(handleFields)->Belt.List.toArray;

        {"type": "section", "fields": fields, "accessory": accessory}
        |> Obj.magic;
      };
    };
  };

  type fields = {
    accessory: Accessory.t,
    fields: list(Field.t),
  };

  type t = [
    | `Divider
    | `FieldsWithImage(fields)
    | `Actions(list(Actions.t))
    | `Section(string)
  ];

  let make: list(t) => array(Js.t('a)) =
    blocks => {
      blocks
      ->Belt.List.map(block =>
          switch (block) {
          | `Divider => Divider.make()
          | `FieldsWithImage({accessory, fields}) =>
            Field.WithImage.make(~accessory, ~fields)
          | `Actions(elements) => Actions.make(~elements)
          | `Section(text) => Section.make(~text)
          }
        )
      ->Belt.List.toArray;
    };
};

module User = {
  let make = id => "<@" ++ id ++ ">";
};

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

module Msg = {
  let make = message => {
    Belt.Result.Ok(Result.Message(Block.make(message)));
  };
};
