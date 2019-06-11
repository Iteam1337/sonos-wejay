type t = {
  command: Command.t,
  args: string,
};

let decode = json =>
  Ezjsonm.{
    command: get_string(find(json, ["command"])) |> Command.parse,
    args: get_string(find(json, ["args"])),
  };
