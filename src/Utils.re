let trackInfo = track => track##artist ++ " - " ++ track##title;
let parseDuration = duration => duration *. 1000. |> Duration.parse;
let cleanFloat = value => value |> int_of_float |> string_of_int;
