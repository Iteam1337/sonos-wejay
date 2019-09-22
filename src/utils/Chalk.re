type chalk;

[@bs.module "chalk"] external hex: string => chalk = "hex";

[@bs.send] external bold: (chalk, string) => chalk = "bold";

[@bs.module "chalk"] external green: string => string = "green";
