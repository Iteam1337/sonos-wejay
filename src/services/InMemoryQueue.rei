type action =
  | AddTrack((option(string), string))
  | ClearQueue
  | RemoveTrack((option(string), string));
type t;
type users;
type trackWithUser = {
  id: string,
  lastPlay: float,
  track: string,
};

let make: action => unit;
let nextTrack: unit => option(trackWithUser);
let currentTrack: unit => option(trackWithUser);
let getQueue: unit => array(trackWithUser);
