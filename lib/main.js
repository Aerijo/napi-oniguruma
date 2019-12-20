function resolve_native(name) {
  try {
    return require(`../build/Release/${name}.node`);
  } catch {
    return require(`../build/Debug/${name}.node`);
  }
}

const {OnigScanner, OnigString} = resolve_native("oniguruma_scanner");

class OnigRegExp {
  constructor(source) {
    this.source = source;
    this.scanner = new OnigScanner([this.source]);
  }

  captureIndicesForMatch(string, match) {
    if (match == null) {
      return null;
    }
    const captures = match.captureIndices;
    for (let i = 0; i < captures.length; i++) {
      const capture = captures[i];
      capture.match = string.slice(capture.start, capture.end);
    }
    return captures;
  }

  searchSync(string, startIndex) {
    const result = this.scanner.findNextMatchSync(string, startIndex || 0);
    return this.captureIndicesForMatch(string, result);
  }

  searchCb(string, startIndex, callback) {
    if (typeof startIndex === "function") {
      callback = startIndex;
      startIndex = 0;
    }

    this.scanner.findNextMatchCb(string, startIndex, (err, captures) => {
      callback(err, this.captureIndicesForMatch(string, captures));
    });
  }

  search(string, startIndex) {
    if (typeof startIndex === "function") {
      callback = startIndex;
      startIndex = 0;
    }

    return this.scanner.findNextMatch(string, startIndex)
      .then(captures => this.captureIndicesForMatch(string, captures));
  }

  testSync(string) {
    return this.scanner.findNextMatchSync(string) != null;
  }

  testCb(string, callback) {
    this.scanner.findNextMatchCb(string, (err, result) => {
      callback(err, result != null);
    });
  }

  test(string) {
    return this.scanner.findNextMatch(string).then(captures => captures != null);
  }
}

OnigString.prototype.substring = function (start, end) {
  return this.content.substring(start, end);
}

OnigString.prototype.slice = function (start, end) {
  return this.content.slice(start, end);
}

OnigString.prototype.toString = function () {
  return this.content;
}

Object.defineProperty(OnigString.prototype, 'length', {
  get() {
    return this.content.length;
  }
});

module.exports = {
  OnigRegExp,
  OnigScanner,
  OnigString,
};
