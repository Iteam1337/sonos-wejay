open Lwt.Infix;

module Slack = {
  let send_message = (~channel, ~user as _u, ~text) =>
    Cohttp_lwt_unix.Client.post(
      ~body=
        Cohttp_lwt.Body.of_string(
          Ezjsonm.(
            dict([
              ("channel", string(channel)),
              ("username", string("Wejay")),
              ("text", string(text)),
              ("mrkdwn", bool(true)),
            ])
            |> to_string
          ),
        ),
      ~headers=
        Cohttp.Header.of_list([
          ("content-type", "application/json"),
          ("Authorization", Format.sprintf("Bearer %s", Config.slack_token)),
        ]),
      Uri.of_string("https://slack.com/api/chat.postMessage"),
    )
    >|= (
      ((response, _body)) =>
        switch (response) {
        | {Cohttp.Response.status: `OK, _} =>
          Ezjsonm.(dict([("success", bool(true))]))
        | _ => Ezjsonm.(dict([("success", bool(false))]))
        }
    );

  let send_message_with_attachments =
      (~channel, ~user as _u, ~text, ~attachments) =>
    Cohttp_lwt_unix.Client.post(
      ~body=
        Cohttp_lwt.Body.of_string(
          Ezjsonm.(
            dict([
              ("channel", string(channel)),
              ("username", string("Wejay")),
              ("attachments", attachments),
              ("text", string(text)),
              ("mrkdwn", bool(true)),
            ])
            |> to_string
          ),
        ),
      ~headers=
        Cohttp.Header.of_list([
          ("content-type", "application/json"),
          ("Authorization", Format.sprintf("Bearer %s", Config.slack_token)),
        ]),
      Uri.of_string("https://slack.com/api/chat.postMessage"),
    )
    >|= (
      ((response, _body)) =>
        switch (response) {
        | {Cohttp.Response.status: `OK, _} =>
          Ezjsonm.(dict([("success", bool(true))]))
        | _ => Ezjsonm.(dict([("success", bool(false))]))
        }
    );

  let track_field = ((t, v)) =>
    Ezjsonm.(
      dict([
        ("title", string(t)),
        ("value", string(v)),
        ("short", bool(true)),
      ])
    );

  let track =
      ({albumName, artist, cover, duration, name, _}: Decode.Spotify.Track.t) =>
    Ezjsonm.(
      dict([
        ("thumb_url", string(cover)),
        (
          "fields",
          [
            ("Artist", artist),
            ("Title", name),
            ("Album", albumName),
            ("Duration", duration |> string_of_int),
          ]
          |> list(track_field),
        ),
      ])
    );

  let tracks = tracks => Ezjsonm.(tracks |> list(track));

  let help = () =>
    [
      "*Queue and search*\n",
      "```",
      "currentqueue|getqueue|cq|gq _________ Display upcoming tracks",
      "fullqueue|fq ________________________ Display all tracks currently in the queue",
      "nowplaying|np _______________________ Display what's playing right now",
      "queue|q <Spotify URI> _______________ Queue a track using a Spotify URI",
      "search|s <your search string> _______ Search Spotify for a track",
      "```",
      "\n*Spotify copy/paste*",
      "In Spotify select one or more tracks and copy their links using `cmd + c` (MacOS) or `ctrl + c` (Windows).",
      "Then paste that information straight to me and I'll queue them for you. :notes:",
      "\n*Play control*",
      "```",
      "clear ______________________ Reset playlist",
      "mute _______________________ Mute",
      "next _______________________ Skip one track forward",
      "pause ______________________ Pause",
      "play _______________________ Play",
      "playtrack <trackNumber> ____ Play track number",
      "previous ___________________ Go back one track",
      "unmute _____________________ Unmute",
      "volume <0-100> _____________ Set volume, if no number is provided displays current volume",
      "```",
      "\n*Misc*",
      "```",
      "blame _____________ Who added the track?",
      "help ______________ Display this message",
      "mostplayed ________ List the most played tracks",
      "toplist ___________ List top Wejayers",
      "```",
      "\nThere's also some hidden easter eggs :hatching_chick:",
    ]
    |> String.concat("\n");

  let reply = (command: Command.t, payload, event) => {
    open Decode.Slack.Event;

    let send_payload = send_message(~user=event.user, ~channel=event.channel);

    let send_payload_wa =
      send_message_with_attachments(
        ~user=event.user,
        ~channel=event.channel,
        ~text="Hello",
      );

    let response =
      switch (command, payload) {
      | (Search, Ok(`Tracks(t))) =>
        tracks(t) |> (attachments => send_payload_wa(~attachments))
      | _ => help() |> (text => send_payload(~text))
      };
    response;
  };
};

module Api = {
  let tracks = json => Decode.Spotify.Tracks.(json |> to_json);
  let track = json => Decode.Spotify.Track.(json |> to_json);
  let help = () => Ezjsonm.(dict([("data", string(Slack.help()))]));

  let unknown_command = () => Ezjsonm.(dict([("success", bool(false))]));

  let reply = (command, payload) => {
    Command.(
      switch (command, payload) {
      | (Search, Ok(`Tracks(t))) => tracks(t)
      | (Queue, Ok(`Track(t))) => track(t)
      | (Help, _) => help()
      | _ => unknown_command()
      }
    );
  };
};

type t = [ | `Api | `Slack(Decode.Slack.Event.event)];

let reply = (reply_type, (command, payload)) =>
  switch (reply_type) {
  | `Api => Lwt.return(Api.reply(command, payload))
  | `Slack(event) => Slack.reply(command, payload, event)
  };
