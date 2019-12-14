const assert = require('assert');
const {OnigScanner} = require("../");

describe("OnigScanner", () => {
  it("exists", () => {
    assert.doesNotThrow(() => { new OnigScanner([]); });
  })
});
