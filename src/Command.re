type t =
  | Search
  | Unknown;

let parse =
  fun
  | "search" => Search
  | _ => Unknown;
