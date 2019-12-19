#ifndef COMMON_H
#define COMMON_H

#include <node_api.h>
#include <stdlib.h>

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

/**
 * Convenience macro to catch and throw any
 * errors encountered, instead of manually
 * checking status or verifying inputs each
 * time
 */
#define NAPI_CALL(env, call)     \
  do {                           \
    napi_status status = (call); \
    if (status != napi_ok) {     \
      return raise_napi_error(env); \
    }                            \
  } while(0)

#define NAPI_CALL_RETURN_VOID(env, call)     \
  do {                           \
    napi_status status = (call); \
    if (status != napi_ok) {     \
      raise_napi_error(env); \
      return; \
    }                            \
  } while(0)

#define NAPI_PROPAGATE_FAILURE(call) \
  do { \
    if ((call) != napi_ok) {     \
      return call; \
    } \
  } while (0)

napi_value raise_napi_error(napi_env env);

#define DECLARE_NAPI_METHOD(name, callback) {name, NULL, callback, NULL, NULL, NULL, napi_default, NULL}

napi_value get_js_utf16_string(napi_env env, napi_value js_string, char** buffer, size_t* bytes);

#endif
