type t =
  | Search
  | Unknown;

let of_string =
  fun
  | "search" => Search
  | _ => Unknown;

let to_string =
  fun
  | Search => "search"
  | Unknown => "unknown";
