[@bs.module "@wejay/spotify"]
external getTrack: string => Js.Promise.t(Js.Json.t) = "getTrack";

[@bs.module "@wejay/spotify"]
external search: string => Js.Promise.t(Js.Json.t) = "search";
