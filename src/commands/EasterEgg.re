type t =
  | FreeBird
  | Friday
  | IteamClassics
  | Rednex
  | Shoreline
  | Slowdance
  | Tequila
  | WWW;

module Track = {
  open SpotifyUtils;

  let freeBird = toUri("4qsAYBCJnu2OkTKUVbbOF1");
  let friday = toUri("4fK6E2UywZTJIa5kWnCD6x");
  let shoreline = toUri("77jVczOFXfbdugN4djsIqs");
  let tequila = toUri("5gJKsGij5oGt5H5RSFYXPa");
  let worldwideweb = toUri("597NerobfkV9DhKCySXXWF");
  let rednex = toUri("06hsdMbBxWGqBO0TV0Zrkf");

  let easterEggTracks = [
    freeBird,
    friday,
    shoreline,
    tequila,
    worldwideweb,
    rednex,
  ];
};

module Playlist = {
  open SpotifyUtils;

  let iteamClassics =
    toPlaylistUri(~user="believer", ~id="445NQ4LkJFtBsHUOdr3LFI");
  let slowdance =
    toPlaylistUri(~user="believer", ~id="5DQzhEf0U4Lji5kvXnPYSy");
};

let run =
  Queue.(
    fun
    | IteamClassics => Playlist.iteamClassics->AsLastTrack.make()
    | FreeBird => Track.freeBird->next
    | Friday =>
      Utils.isFriday()
        ? Track.friday->next
        : `Ok(Slack.Block.make([`Section("Sorry, it's not Friday")]))
          |> Js.Promise.resolve
    | Rednex => Track.rednex->next
    | Shoreline => Track.shoreline->next
    | Slowdance => Playlist.slowdance->AsLastTrack.make()
    | Tequila => Track.tequila->next
    | WWW => Track.worldwideweb->next
  );

module Test = {
  type t =
    | EasterEgg
    | RegularTrack;

  let fromBool =
    fun
    | true => EasterEgg
    | false => RegularTrack;

  let make = continuation => {
    Js.Promise.(
      Services.getCurrentTrack()
      |> then_(({uri}: Sonos.Decode.currentTrackResponse) => {
           let isEasterEgg =
             Track.easterEggTracks
             ->Belt.List.some(track =>
                 track == Utils.sonosUriToSpotifyUri(uri)
               )
             ->fromBool;

           switch (isEasterEgg) {
           | EasterEgg =>
             `Ok(Slack.Block.make([`Section(Message.cantSkipEasterEgg)]))
             |> resolve
           | RegularTrack => continuation
           };
         })
    );
  };
};
