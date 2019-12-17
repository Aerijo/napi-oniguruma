const assert = require('assert');
const {OnigScanner} = require("../");

describe("OnigScanner", function() {
  const inspect = ({scanner, promises}, text, start, expected_err, expected_captures) => {
    promises.push(new Promise(resolve => {
      scanner.findNextMatchCb(text, start, (err, captures) => {
        assert.deepStrictEqual(err, expected_err);
        if (captures !== null) {
          delete captures.scanner;
        }
        assert.deepStrictEqual(captures, expected_captures);
        resolve();
      });
    }));

    // TODO: Implement promise API
    // promises.push(new Promise(async resolve => {
    //   scanner.findNextMatch(text, start).then(captures => {
    //     assert.deepStrictEqual(captures, expected_captures);
    //   }).catch(err => {
    //     assert.deepStrictEqual(err, expected_err);
    //   }).finally(() => {
    //     resolve();
    //   });
    // }));

    assert.deepStrictEqual(scanner.findNextMatchSync(text, start), expected_captures);
  }

  it("returns consistent results across the different interfaces", function() {
    const scanner = new OnigScanner(["a", "b", "c"]);
    const promises = [];
    const c = {scanner, promises};

    inspect(c, "x", 0, null, null);

    inspect(c, "xxaxxbxxc", 0, null, {
      index: 0,
      captureIndices: [{start: 2, end: 3, length: 1}],
    });

    inspect(c, "xxaxxbxxc", 4, null, {
      index: 1,
      captureIndices: [{start: 5, end: 6, length: 1}],
    });

    inspect(c, "xxaxxbxxc", 7, null, {
      index: 2,
      captureIndices: [{start: 8, end: 9, length: 1}],
    });

    inspect(c, "xxaxxbxxc", 9, null, null);

    return Promise.all(promises);
  });

  describe("when the string searched contains unicode", function() {
    it("handles regular width unicode", function() {
      assert.strictEqual("…".length, 1);

      const scanner = new OnigScanner(["1", "2"]);
      const promises = [];
      const c = {scanner, promises};

      inspect(c, "ab…cde21", 5, null, {
        index: 1,
        captureIndices: [{start: 6, end: 7, length: 1}],
      });

      return Promise.all(c.promises);
    });

    it("handles extra wide unicode", function() {
      assert.strictEqual("🇦🇺".length, 4); // Flag is made of two 2-byte code units

      const s = new OnigScanner(["1", "2"]);
      const c = {
        promises: [],
        scanner: s,
      };

      inspect(c, "ab🇦🇺cde21", 5, null, {
        index: 1,
        captureIndices: [{start: 9, end: 10, length: 1}],
      });

      return Promise.all(c.promises);
    });
  });
});
