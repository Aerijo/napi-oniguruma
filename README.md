[![Build Status](https://travis-ci.com/Aerijo/napi-oniguruma.svg?branch=master)](https://travis-ci.com/Aerijo/napi-oniguruma)
[![Dependency Status](https://david-dm.org/Aerijo/napi-oniguruma.svg)](https://david-dm.org/Aerijo/napi-oniguruma)
[![DevDependency Status](https://david-dm.org/Aerijo/napi-oniguruma/dev-status.svg)](https://david-dm.org/Aerijo/napi-oniguruma/?type=dev)

# napi-oniguruma

NAPI bindings for the Oniguruma regex library


## Thread safety

The entry and exit points of all async methods run on the main thread, so are inherently safe. The biggest concern is the work that gets done in the threadpool. In particular, the regex cannot cache results without protection. To fix this, we only update the cache during sync searches (where everything runs on the main thread). The goal will be to eventually add atomic operations to update values, even during async work (but Windows VS doesn't seem to support atomics). Maybe even make the cache a separate object, so a regex can have multiple caches, etc.


## Indexing

The API supports starting from a given index within the string. This has been interpreted as JS string index, and not unicode code point or similar. Hopefully support for actual code point indexing can be added.
