let nothingIsPlaying = "Nothing is currently playing, add a track using `search <your track>`";

let emptyQueue = "The queue is empty, add a track using `search <your track>`";

let noPlays = "No plays :sad_panda:";

let help =
  [|
    "*Queue and search*\n",
    "```",
    "currentqueue|getqueue|cq|gq _________ Display upcoming tracks",
    "fullqueue|fq ________________________ Display all tracks currently in the queue",
    "nowplaying|np _______________________ Display what's playing right now",
    "queue|q <Spotify URI> _______________ Queue a track using a Spotify URI",
    "search|s <your search string> _______ Search Spotify for a track",
    "```",
    "\n*Spotify copy/paste*",
    "In Spotify select one or more tracks and copy their links using `cmd + c` (MacOS) or `ctrl + c` (Windows).",
    "Then paste that information straight to me and I'll queue them for you. :notes:",
    "\n*Play control*",
    "```",
    "clear ______________________________ Reset playlist",
    "mute _______________________________ Mute",
    "next|skip __________________________ Skip one track forward",
    "pause ______________________________ Pause",
    "play _______________________________ Play",
    "playtrack <trackNumber> ____________ Play track number",
    "playlatest|pl  _____________________ Play latest track",
    "previous ___________________________ Go back one track",
    "unmute _____________________________ Unmute",
    "volume <0-100> _____________________ Set volume, if no number is provided displays current volume",
    "remove|rm <numberOfTracks> <index>__ Remove tracks",
    "```",
    "\n*Misc*",
    "```",
    "blame <trackNumber> ___ Who added the track?",
    "help __________________ Display this message",
    "mostplayed ____________ List the most played tracks",
    "toplist _______________ List top Wejayers",
    "```",
    "\nThere's also some hidden easter eggs :hatching_chick:",
  |]
  ->Utils.joinWithNewline;

let unknownCommand = command =>
  [|
    {j|Sorry, the command: *"$command"* is not available.|j},
    "Type *\"help\"* in order to get all available commands! :face_with_monocle:",
  |]
  ->Utils.joinWithNewline;

let thisIsWejay = "This is Wejay!\nhttps://media.giphy.com/media/Ny4Ian52lZDz2/giphy.gif";

let cantSkipEasterEgg = "Can't skip this track :hatching_chick:";

let trackExistsInQueue = "Hey, this track already exists in the queue!";

let botRequest = "That's a bot request :robot:";

let unhandledCommand = "This is not the command you are looking for";
