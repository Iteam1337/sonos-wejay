type action =
  | AddTrack((option(string), string))
  | ClearQueue
  | RemoveTrack((option(string), string));
type t;
type users;

let make: action => unit;
let nextTrack: unit => option(string);
let currentTrack: unit => option(string);
let getQueue: unit => array(string);
