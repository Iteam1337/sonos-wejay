[@bs.module "date-fns"]
external format: (Js.Date.t, string) => string = "format";

module Format = {
  let date = dateString =>
    dateString->Js.Date.fromString->format("yyyy-MM-dd");

  let timestamp = timestamp =>
    timestamp->Js.Date.fromFloat->format("yyyy-MM-dd");
};

module Day = {
  type t =
    | Monday
    | Tuesday
    | Wednesday
    | Thursday
    | Friday
    | Saturday
    | Sunday;

  let make =
    fun
    | Monday => 1.
    | Tuesday => 2.
    | Wednesday => 3.
    | Thursday => 4.
    | Friday => 5.
    | Saturday => 6.
    | Sunday => 0.;
};

let isDay = day =>
  switch (Js.Date.make() |> Js.Date.getDay) {
  | d when d === Day.make(day) => true
  | _ => false
  };

module Month = {
  type t =
    | January
    | February
    | March
    | April
    | May
    | June
    | July
    | August
    | September
    | October
    | November
    | December;

  let make =
    fun
    | January => 0.
    | February => 1.
    | March => 2.
    | April => 3.
    | May => 4.
    | June => 5.
    | July => 6.
    | August => 7.
    | September => 8.
    | October => 9.
    | November => 10.
    | December => 11.;
};

let isMonth = month =>
  switch (Js.Date.make() |> Js.Date.getMonth) {
  | m when m === Month.make(month) => true
  | _ => false
  };
