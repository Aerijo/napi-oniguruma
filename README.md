# napi-oniguruma

NAPI bindings for the Oniguruma regex library


// TODO: Handle scenario:
  1. OnigScanner calls async find next match
  2. Scanner goes out of scope and is garbage collected
  3. Native code attached to scanner is freed (regexes)
  4. Async work is done on invalid regex pointers

Solutions:
  1. Have atomic counter for regex uses. Last to finish (scanner destruction vs async worker(s)) cleans up
  2. Guarantee scanner remains live, by maintaing reference to it in JS land
