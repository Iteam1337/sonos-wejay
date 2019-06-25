type t =
  | Help
  | Search
  | Queue
  | Unknown;

let of_string =
  fun
  | "search" => Search
  | "help" => Help
  | "queue" => Queue
  | _ => Unknown;

let to_string =
  fun
  | Help => "help"
  | Search => "search"
  | Queue => "queue"
  | Unknown => "unknown";
