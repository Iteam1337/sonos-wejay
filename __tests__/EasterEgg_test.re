open Jest;
open Expect;
open EasterEgg;

[@bs.module "jest-date-mock"] external clear: unit => unit = "clear";
[@bs.module "jest-date-mock"] external advanceTo: float => unit = "advanceTo";

afterEach(clear);

describe("Tracks", () => {
  test("Lynyrd Skynyrd - Free bird", () =>
    expect(Track.freeBird) |> toEqual("spotify:track:4qsAYBCJnu2OkTKUVbbOF1")
  );

  test("Rebecca Black - Friday", () =>
    expect(Track.friday) |> toEqual("spotify:track:4fK6E2UywZTJIa5kWnCD6x")
  );

  test("Broder Daniel - Shoreline", () =>
    expect(Track.shoreline)
    |> toEqual("spotify:track:77jVczOFXfbdugN4djsIqs")
  );

  test("The Champs - Tequila", () =>
    expect(Track.tequila) |> toEqual("spotify:track:5gJKsGij5oGt5H5RSFYXPa")
  );

  test("Nick Borgen - World Wide Web", () =>
    expect(Track.worldwideweb)
    |> toEqual("spotify:track:597NerobfkV9DhKCySXXWF")
  );
});

describe("Playlists", () => {
  test("Iteam classics", () =>
    expect(Playlist.iteamClassics)
    |> toEqual("spotify:user:believer:playlist:445NQ4LkJFtBsHUOdr3LFI")
  );

  test("Slowdance music", () =>
    expect(Playlist.slowdance)
    |> toEqual("spotify:user:believer:playlist:5DQzhEf0U4Lji5kvXnPYSy")
  );
});
