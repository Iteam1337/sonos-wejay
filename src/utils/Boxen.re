[@bs.deriving {jsConverter: newType}]
type borderStyle = [
  | `single
  | `round
  | `double
  | `bold
  | `singleDouble
  | `doubleSingle
  | `classic
];

[@bs.deriving abstract]
type options = {
  padding: option(int),
  margin: option(int),
  borderColor: option(string),
  borderStyle: abs_borderStyle,
  dimBorder: option(bool),
};

[@bs.module] external boxen: (string, options) => string = "boxen";

let make =
    (~value, ~padding=None, ~margin=None, ~borderColor=None, ~borderStyle, ()) => {
  boxen(
    value,
    options(
      ~borderColor,
      ~dimBorder=Some(false),
      ~padding,
      ~margin,
      ~borderStyle=borderStyleToJs(borderStyle),
    ),
  );
};
