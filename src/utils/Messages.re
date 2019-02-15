let nothingIsPlaying = "Nothing is currently playing, add a track using `search <your track>`";

let emptyQueue = "The queue is empty, add a track using `search <your track>`";

let help =
  [
    "*Queue and search*\n",
    "```",
    "currentqueue|getqueue|cq|gq _________ Display upcoming tracks",
    "fullqueue|fq ________________________ Display all tracks currently in the queue",
    "nowplaying|np _______________________ Display what's playing right now",
    "queue|q <Spotify URI> _______________ Queue a track using a Spotify URI",
    "search|s <your search string> _______ Search Spotify for a track",
    "library|l <your search string> ______ Search in music library",
    "```",
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

let unknownCommand = command =>
  [
    "Sorry, the command: *\"" ++ command ++ "\"* is not available.",
    "Type *\"help\"* in order to get all available commands! :face_with_monocle:",
  ]
  |> String.concat("\n");

let thisIsWejay = "This is Wejay!\nhttps://media.giphy.com/media/Ny4Ian52lZDz2/giphy.gif";
