const oniguruma = require("../src/main.js");
const util = require("util");

console.log(oniguruma);

const o = new oniguruma.OnigScanner(["a", "b", "c"]);

console.log(o);

o.bar();
