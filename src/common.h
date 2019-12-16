#ifndef COMMON_H
#define COMMON_H

#include <node_api.h>

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

napi_value raise_napi_error(napi_env env) {
  const napi_extended_error_info* error_info = NULL;
  napi_get_last_error_info((env), &error_info);
  bool is_pending;
  napi_is_exception_pending((env), &is_pending);
  if (!is_pending) {
    const char* message = (error_info->error_message == NULL)
        ? "empty error message"
        : error_info->error_message;
    napi_throw_error((env), NULL, message);
  }
  return NULL;
}

#define DECLARE_NAPI_METHOD(name, callback) {name, NULL, callback, NULL, NULL, NULL, napi_default, NULL}

#define NAPI_GET_STRING_U16(env, js_string, buffer_name, length_name) \
  size_t length_name; \
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, NULL, 0, &length_name)); \
  length_name = (length_name + 1) * 2; \
  char* buffer_name = malloc(length_name); \
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, (char16_t*) buffer_name, length_name, &length_name));


napi_value get_js_utf16_string(napi_env env, napi_value js_string, char** buffer, size_t* length) {
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, NULL, 0, length));
  *length = (*length) * 2;
  *buffer = malloc(*length);
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, (char16_t*) *buffer, *length, NULL));
  return NULL;
}

#endif
