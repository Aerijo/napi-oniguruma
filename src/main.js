const oniguruma = require("../build/Release/oniguruma_scanner.node");

const OnigScanner = oniguruma.OnigScanner;
const OnigString = oniguruma.OnigString;

class OnigRegExp {
  constructor(source) {
    this.source = source;
    this.scanner = new OnigScanner([this.source]);
  }
}

OnigString.prototype.substring = function (start, end) {
  return this.content.substring(start, end);
}

OnigString.prototype.toString = function () {
  return this.content;
}

Object.defineProperty(OnigString.prototype, 'length', {
  get() {
    return this.content.length;
  }
})

module.exports = {
  OnigRegExp,
  OnigScanner,
  OnigString,
};
