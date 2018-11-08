open Jest;
open Sonos;

describe("Sonos", () =>
  Expect.(
    describe("#SonosDecode", () => {
      describe("#currentPlayingState", () => {
        test("stopped", () =>
          expect(SonosDecode.currentPlayingState("stopped")) |> toEqual(Stopped)
        );

        test("playing", () =>
          expect(SonosDecode.currentPlayingState("playing")) |> toEqual(Playing)
        );

        test("undefined", () =>
          expect(SonosDecode.currentPlayingState("undefined")) |> toEqual(UnknownState)
        );
      })
    })
  )
);
