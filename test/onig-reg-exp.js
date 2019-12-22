const assert = require("assert");
const {OnigRegExp} = require("../");

// TODO: Generalise
const inspect = ({regex, promises}, text, start, expected_err, expected_captures) => {
  promises.push(new Promise(resolve => {
    regex.searchCb(text, start, (err, captures) => {
      assert.deepStrictEqual(err, expected_err);
      assert.deepStrictEqual(captures, expected_captures);
      resolve();
    });
  }));

  promises.push(new Promise(resolve => {
    regex.search(text, start).then(captures => {
      assert.deepStrictEqual(captures, expected_captures);
    }).catch(err => {
      assert.deepStrictEqual(err, expected_err);
    }).finally(() => {
      resolve();
    });
  }));

  assert.deepStrictEqual(regex.searchSync(text, start), expected_captures);

  return promises;
}

describe("OnigRegExp", function() {
  it("retrieves the substrings of each match", function() {
    const regex = new OnigRegExp("bar");
    const promises = [];
    const c = {regex, promises};
    inspect(c, "foobarbaz", 0, null, [{index: 0, start: 3, end: 6, length: 3, match: "bar"}]);
    inspect(c, "helloworld", 0, null, null);

    return promises;
  });

  describe("when testing for a match", function() {
    it("returns true only if the string matches the pattern", function() {
      const regex = new OnigRegExp("a[b-d]c");
      assert.strictEqual(regex.testSync("abc"), true);
      assert.strictEqual(regex.testSync("acc"), true);
      assert.strictEqual(regex.testSync("agc"), false);
      assert.strictEqual(regex.testSync("acb"), false);
    });
  });
});
