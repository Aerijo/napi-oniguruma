const oniguruma = require("../build/Release/oniguruma.node");

module.exports = oniguruma;
//
// const OnigScanner = oniguruma.OnigScanner;
// const OnigString = oniguruma.OnigString;
//
// class OnigRegExp {
//   constructor(source) {
//     this.source = source;
//     this.scanner = new OnigScanner([this.source]);
//   }
// }
//
// OnigScanner.prototype.findNextMatchSync = function (string, startPosition) {
//   if (startPosition == undefined) {
//     startPosition = 0;
//   }
//
//   string = this.convertToString(string)
//   startPosition = this.convertToNumber(startPosition)
//
//   let match = this._findNextMatchSync(string, startPosition)
//   if (match) match.scanner = this
//   return match
// }
//
// OnigScanner.prototype.convertToString = function (value) {
//   if (value.constructor == OnigString) {
//     return value;
//   }
//
//   if (typeof value !== "string") {
//     throw new Error(`Expected string in OnigScanner#convertToString, got ${typeof value}`)
//   }
//
//   return value;
// }
