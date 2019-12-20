const assert = require("assert");
const {OnigScanner, OnigString} = require("../");

const inspect = ({scanner, promises}, text, start, expected_err, expected_captures) => {
  promises.push(new Promise(resolve => {
    scanner.findNextMatchCb(text, start, (err, captures) => {
      assert.deepStrictEqual(err, expected_err);
      assert.deepStrictEqual(captures, expected_captures);
      resolve();
    });
  }));

  promises.push(new Promise(resolve => {
    scanner.findNextMatch(text, start).then(captures => {
      assert.deepStrictEqual(captures, expected_captures);
    }).catch(err => {
      assert.deepStrictEqual(err, expected_err);
    }).finally(() => {
      resolve();
    });
  }));

  assert.deepStrictEqual(scanner.findNextMatchSync(text, start), expected_captures);

  return promises;
}

describe("OnigScanner", function() {
  describe("when handling parameters", function() {
    it("accepts a start position to all interfaces", function() {
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

    it("defaults to a start index of 0 if not provided", async function() {
      const scanner = new OnigScanner(["a"]);

      assert.deepStrictEqual(scanner.findNextMatchSync("aaa"), {
        index: 0,
        captureIndices: [{start: 0, end: 1, length: 1}],
      });

      assert.deepStrictEqual(await scanner.findNextMatch("aaa"), {
        index: 0,
        captureIndices: [{start: 0, end: 1, length: 1}],
      });

      await new Promise(resolve => {
        scanner.findNextMatchCb("aaa", (err, captures) => {
          assert.strictEqual(err, null);
          assert.deepStrictEqual(captures, {
            index: 0,
            captureIndices: [{start: 0, end: 1, length: 1}],
          });
          resolve();
        });
      });
    });

    it("clamps out of bounds start index", function() {
      const scanner = new OnigScanner(["X"]);
      const promises = [];
      const c = {scanner, promises};

      inspect(c, "fooXbar", -100, null, {
        index: 0,
        captureIndices: [{start: 3, end: 4, length: 1}],
      });

      inspect(c, "fooXbar", 100, null, null);

      return Promise.all(promises);
    });
  });

  describe("when the regular expression contains double byte characters", function() {
    it("returns the correct match length", function() {
      const scanner = new OnigScanner(["Возврат"]);
      return Promise.all(inspect({scanner, promises: []}, "Возврат long_var_name;", 0, null, {
        index: 0,
        captureIndices: [{start: 0, end: 7, length: 7}]
      }));
    });
  });

  describe("when the input string contains unicode", function() {
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

  describe("when the input string contains invalid surrogate pairs", function() {
    it("interprets them as a code point", function() {
      const scanner = new OnigScanner(["X"]);
      const promises = [];
      const c = {scanner, promises};

      inspect(c, `X${String.fromCharCode(0xd83c)}X`, 0, null, {
        index: 0,
        captureIndices: [{start: 0, end: 1, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xd83c)}X`, 1, null, {
        index: 0,
        captureIndices: [{start: 2, end: 3, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xd83c)}X`, 2, null, {
        index: 0,
        captureIndices: [{start: 2, end: 3, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xdfff)}X`, 0, null, {
        index: 0,
        captureIndices: [{start: 0, end: 1, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xdfff)}X`, 1, null, {
        index: 0,
        captureIndices: [{start: 2, end: 3, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xdfff)}X`, 2, null, {
        index: 0,
        captureIndices: [{start: 2, end: 3, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xd800)}${String.fromCharCode(0xdc00)}X`, 2, null, {
        index: 0,
        captureIndices: [{start: 3, end: 4, length: 1}],
      });

      inspect(c, `X${String.fromCharCode(0xdbff)}${String.fromCharCode(0xdfff)}X`, 2, null, {
        index: 0,
        captureIndices: [{start: 3, end: 4, length: 1}],
      });

      return Promise.all(promises);
    });
  });

  describe("when a regular expression contains \\G (current search start anchor)", function() {
    it("does not get cached", async function() {
      const str = new OnigString("axxax");
      const scanner = new OnigScanner(["\\Gx"]);
      assert.deepStrictEqual(scanner.findNextMatchSync(str, 0), null);
      assert.deepStrictEqual(scanner.findNextMatchSync(str, 1), {
        index: 0,
        captureIndices: [{start: 1, end: 2, length: 1}],
      });
      assert.deepStrictEqual(scanner.findNextMatchSync(str, 2), {
        index: 0,
        captureIndices: [{start: 2, end: 3, length: 1}],
      });
      assert.deepStrictEqual(scanner.findNextMatchSync(str, 3), null);
      assert.deepStrictEqual(scanner.findNextMatchSync(str, 4), {
        index: 0,
        captureIndices: [{start: 4, end: 5, length: 1}],
      });
    });
  });
});
