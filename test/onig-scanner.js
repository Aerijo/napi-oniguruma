const assert = require('assert');
const {OnigScanner} = require("../");

// describe("OnigScanner", function () {
//   it("exists", () => {
//     assert.doesNotThrow(() => { new OnigScanner([]); });
//   });
//
//   it("test", function () {
    const o = new OnigScanner(["foo", "bar"]);
    o.findNextMatchCb("fobar", (a, b) => {
      console.log(a, b)
    });
//   });
// });
