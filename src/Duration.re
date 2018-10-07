let addLeadingZero = value =>
  value < 10.0 ?
    "0" ++ (value |> int_of_float |> string_of_int) :
    value |> int_of_float |> string_of_int;

let parseToMinutes = (seconds, hours) => seconds /. 60.0 -. 60.0 *. hours;
let parseOutputSeconds = seconds =>
  addLeadingZero(mod_float(seconds /. 60.0, 1.0) *. 60.0);

let parse = milliseconds => {
  let seconds = milliseconds /. 1000.0;
  let hours = floor(seconds /. 3600.0);
  let secondsOutput = parseOutputSeconds(seconds);
  let minutesOutput = parseToMinutes(seconds, hours);

  switch (hours, minutesOutput, secondsOutput) {
  | (0.0, _, _) =>
    (minutesOutput |> int_of_float |> string_of_int) ++ ":" ++ secondsOutput
  | _ =>
    (hours |> int_of_float |> string_of_int)
    ++ ":"
    ++ addLeadingZero(minutesOutput)
    ++ ":"
    ++ secondsOutput
  };
};
