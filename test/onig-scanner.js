const assert = require('assert');
const {OnigScanner} = require("../");

describe("OnigScanner", function() {
  describe("::findNextMatchSync", function() {
    it("works", function() {
      const s = new OnigScanner(["b", "a"]);
      assert.deepStrictEqual(s.findNextMatchSync("xxab"), {
        index: 1,
        captureIndices: [{
          start: 2,
          end: 3,
          length: 1,
        }],
      });
    });
  });

  describe("::findNextMatchCb", function() {
    const inspectCall = ({promises, scanner}, text, start, cb) => {
      promises.push(new Promise(resolve => {
        scanner.findNextMatchCb(text, start, (err, captures) => {
          cb(err, captures);
          resolve();
        })
      }));
    }

    it("correctly matches ASCII expressions", function() {
      const s = new OnigScanner(["a", "b", "c"]);
      const c = {
        promises: [],
        scanner: s,
      };

      inspectCall(c, "x", 0, (err, captures) => {
        assert.strictEqual(err, null);
        assert.strictEqual(captures, null);
      });

      inspectCall(c, "xxaxxbxxc", 0, (err, captures) => {
        assert.strictEqual(err, null);
        assert.deepStrictEqual(captures, {
          scanner: s,
          index: 0,
          captureIndices: [{
            start: 2,
            end: 3,
            length: 1,
          }],
        });
      });

      inspectCall(c, "xxaxxbxxc", 4, (err, captures) => {
        assert.strictEqual(err, null);
        assert.deepStrictEqual(captures, {
          scanner: s,
          index: 1,
          captureIndices: [{
            start: 5,
            end: 6,
            length: 1,
          }],
        });
      });

      inspectCall(c, "xxaxxbxxc", 7, (err, captures) => {
        assert.strictEqual(err, null);
        assert.deepStrictEqual(captures, {
          scanner: s,
          index: 2,
          captureIndices: [{
            start: 8,
            end: 9,
            length: 1,
          }],
        });
      });

      inspectCall(c, "xxaxxbxxc", 9, (err, captures) => {
        assert.strictEqual(err, null);
        assert.strictEqual(captures, null);
      });

      return Promise.all(c.promises);
    });

    describe("when the string searched contains unicode", function() {
      it("handles regular width unicode", function() {
        const s = new OnigScanner(["1", "2"]);
        const c = {
          promises: [],
          scanner: s,
        };

        inspectCall(c, "ab…cde21", 5, (err, captures) => {
          assert.strictEqual(err, null);
          assert.deepStrictEqual(captures, {
            scanner: s,
            index: 1,
            captureIndices: [{
              start: 6,
              end: 7,
              length: 1,
            }],
          });
        });

        return Promise.all(c.promises);
      });

      it("handles extra wide unicode", function() {
        const s = new OnigScanner(["1", "2"]);
        const c = {
          promises: [],
          scanner: s,
        };

        inspectCall(c, "ab🇦🇺cde21", 5, (err, captures) => { // Flag is made of two 2-byte code units
          assert.strictEqual("🇦🇺".length, 4);
          assert.strictEqual(err, null);
          assert.deepStrictEqual(captures, {
            scanner: s,
            index: 1,
            captureIndices: [{
              start: 9,
              end: 10,
              length: 1,
            }],
          });
        });

        return Promise.all(c.promises);
      });
    });
  });
});
