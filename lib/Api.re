type t = {
  command: Command.t,
  args: string,
};

let decode = json =>
  Yojson.Basic.Util.{
    command: json |> member("command") |> to_string |> Command.of_string,
    args: json |> member("args") |> to_string,
  };

let make = (~args, ~command) => {args, command};
