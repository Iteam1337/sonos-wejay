module LeadingZero = {
  let add = value =>
    switch (value) {
    | v when v < 10. => "0" ++ value->Utils.cleanFloat
    | _ => value->Utils.cleanFloat
    };
};

module Minutes = {
  let make = (h, s) => s /. 60.0 -. 60.0 *. h;
};

module Seconds = {
  let make = seconds =>
    LeadingZero.add(mod_float(seconds /. 60.0, 1.0) *. 60.0);
};

let make = milliseconds => {
  let seconds =
    switch (milliseconds) {
    | ms when ms < 10000. => ms
    | ms => ms /. 1000.
    };

  let h = floor(seconds /. 3600.0);
  let s = Seconds.make(seconds);
  let min = Minutes.make(h, seconds);

  switch (h, min, s) {
  | (0.0, min, s) => min->Utils.cleanFloat ++ ":" ++ s
  | (h, min, s) =>
    h->Utils.cleanFloat ++ ":" ++ LeadingZero.add(min) ++ ":" ++ s
  };
};
