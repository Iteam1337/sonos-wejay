type t = {
  command: Command.t,
  args: string,
};

let decode = json =>
  Ezjsonm.{
    command: get_string(find(json, ["command"])) |> Command.of_string,
    args: get_string(find(json, ["args"])),
  };
