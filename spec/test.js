const oniguruma = require("../src/main.js");
const util = require("util");

console.log(oniguruma);

const o = new oniguruma.OnigScanner(["a", "b", "c"]);

console.log(o);

o.bar();

for (let i = 0; i < 1000000; i++) {

}

// console.log(o);
// console.log(o.foo);
// console.log(o.bar);
// console.log(o.bar());

// const foo = new oniguruma.OnigScanner(3);
// console.log(foo);
// console.log(util.inspect(foo, {colors: true, depth: Infinity}));
// console.log(foo.foo);
// console.log(foo.bar);
//
// // for (let i = 0; i < 10000000000; i++) {
// //   const foo = new oniguruma.OnigScanner(i);
// // }
//
//
// class Foo {
//   constructor() {
//     this.bar = 12;
//   }
// }
//
// console.log(new Foo());
