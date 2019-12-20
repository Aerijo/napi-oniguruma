[![Build Status](https://travis-ci.com/Aerijo/napi-oniguruma.svg?branch=master)](https://travis-ci.com/Aerijo/napi-oniguruma)
[![Dependency Status](https://david-dm.org/Aerijo/napi-oniguruma.svg)](https://david-dm.org/Aerijo/napi-oniguruma)
[![DevDependency Status](https://david-dm.org/Aerijo/napi-oniguruma/dev-status.svg)](https://david-dm.org/Aerijo/napi-oniguruma/?type=dev)

# napi-oniguruma

N-API bindings for the Oniguruma regex library

## Features

This module uses the lastest Oniguruma regex release (6.9.4), and aims to keep it up to date. It uses N-API bindings, allowing the same binary to work across [all future NodeJS versions](https://nodejs.org/en/docs/guides/abi-stability/#n-api). The implementation is based on `node-oniguruma`, but rewritten in C, some interface differences, and with added promise support.

To interface with the regular expressions, there is a choice of
- Synchronous calls with `scanner.findNextMatchSync` (fastest, but blocks the main thread)
- Callbacks with `scanner.findNextMatchCb` (regex match is run on separate thread)
- Promises with `scanner.findNextMatch` (same as callbacks)


## Performance

When I run the `node-oniguruma` benchmarks on it's own library, I get the following results
```
oneline.js
sync:  149652 matches in 1778ms
large.js
sync:  14594 matches in 162ms
Segmentation fault (core dumped)
```

When I run the same benchmarks on this library, I get
```
oneline.js
sync:  149652 matches in 2985ms
large.js
sync:  14594 matches in 186ms
async: 14594 matches in 1055ms
```

They tend to have some variance, but the above figures are fairly typical. From the above results, you can expect a significant performance drop when run on a single line JSON, and a 1.15 times slowdown when searching for `'this', 'var', 'selector', 'window'` on the JQuery source.

In general, a drop in performance is to be expected. N-API is an abstraction over the underlying JS VM, so cannot compete with `node-oniguruma` using the V8 API's. However, what we lose in performance we gain in stability, so you can depend on prebuilt binaries being valid without active maintenance. I am interested in seeing if NAN bindings can also be implemented in this project too though, and comparing their performance to those in `node-oniguruma`.

In a point to this library though, we don't segfault on async operations. Improving the benchmarks to better reflect real usage is a goal for this module.



## Thread safety

The entry and exit points of all async methods run on the main thread, so are inherently safe. The biggest concern is the work that gets done in the threadpool. In particular, the regex cannot cache results without protection. To fix this, we only update the cache during sync searches (where everything runs on the main thread). The goal will be to eventually add atomic operations to update values, even during async work (but Windows VS doesn't seem to support atomics). Maybe even make the cache a separate object, so a regex can have multiple caches, etc.


## Indexing

The API supports starting from a given index within the string. This has been interpreted as JS string index, and not unicode code point or similar. Hopefully support for actual code point indexing can be added.


## NOTE
- There is a bug in Node 10.15.3--12.0.0 that causes a memory leak when using the async (promise and callback) methods.


## TODO
- [X] Refactor C code to reduce duplication
- [ ] Fix the G anchor detection
- [ ] Comprehensive tests
- [ ] Set up proper benchmarks
- [ ] Typescript types
- [ ] ~~Batch the property setting into a single N-API call~~
  - Not done, as could not detect any measurable improvement, and makes the properties invisible to the tests.
