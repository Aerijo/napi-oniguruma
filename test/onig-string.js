const assert = require('assert');
const {OnigString} = require("../");

describe("OnigString", () => {
  it("has a length property", () => {
    assert.equal(new OnigString("abc").length, 3);
    assert.equal(new OnigString("foobar").length, 6);
  });

  it("can be converted back into a string", () => {
    assert.equal(new OnigString("abc").toString(), "abc");
    assert.equal(new OnigString("𝌆é").toString(), "𝌆é");
  });

  it("can retrieve substrings", () => {
    const check = (original, indices) => {
      const onig = new OnigString(original);
      for (const ind of indices) {
        assert.equal(onig.substring(...ind), original.substring(...ind));
      }
    }

    check("abcdef", [[2, 3], [2], [], [-1], [-1, -2], [{}], [null, undefined]]);
    // TODO: variable width substrings
  });

  it("handles invalid arguments", () => {
    assert.throws(() => {
      new OnigString({});
    }, /\bstring\b/);
  });
});
