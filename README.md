# napi-oniguruma

NAPI bindings for the Oniguruma regex library


## Thread safety

The entry and exit points of all async methods run on the main thread, so are inherently safe. The biggest concern is the work that gets done in the threadpool. In particular, the regex cannot cache results without protection. To fix this, we only update the cache during sync searches (where everything runs on the main thread). The goal will be to eventually add atomic operations to update values, even during async work (but Windows VS doesn't seem to support atomics). Maybe even make the cache a separate object, so a regex can have multiple caches, etc.
