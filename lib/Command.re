type t =
  | Help
  | Search
  | Unknown;

let of_string =
  fun
  | "search" => Search
  | "help" => Help
  | _ => Unknown;

let to_string =
  fun
  | Help => "help"
  | Search => "search"
  | Unknown => "unknown";
