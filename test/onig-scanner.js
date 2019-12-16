const assert = require('assert');
const {OnigScanner} = require("../");

describe("OnigScanner", function() {
  it("exists", () => {
    assert.doesNotThrow(() => { new OnigScanner([]); });
  });

  describe("::findNextMatchCb", function() {
    const promises = [];
    const inspectCall = (scanner, text, start, cb) => {
      promises.push(new Promise(resolve => {
        scanner.findNextMatchCb(text, start, (err, captures) => {
          cb(err, captures);
          resolve();
        })
      }));
    }

    it("returns the index of the matching pattern", function() {
      const s = new OnigScanner(["a", "b", "c"]);

      inspectCall(s, "x", 0, (err, captures) => {
        assert.strictEqual(err, null);
        assert.strictEqual(captures, null);
      });

      inspectCall(s, "xxaxxbxxc", 0, (err, captures) => {
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

      inspectCall(s, "xxaxxbxxc", 4, (err, captures) => {
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

      inspectCall(s, "xxaxxbxxc", 7, (err, captures) => {
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

      inspectCall(s, "xxaxxbxxc", 9, (err, captures) => {
        assert.strictEqual(err, null);
        assert.strictEqual(captures, null);
      });

      return Promise.all(promises);
    });
  });
});
