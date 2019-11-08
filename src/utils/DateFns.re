[@bs.module "date-fns"]
external format: (Js.Date.t, string) => string = "format";

module Format = {
  let date = dateString =>
    dateString->Js.Date.fromString->format("yyyy-MM-dd");

  let timestamp = timestamp =>
    timestamp->Js.Date.fromFloat->format("yyyy-MM-dd");
};
