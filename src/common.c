#include "./common.h"

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

napi_value get_js_utf16_string(napi_env env, napi_value js_string, char** buffer, size_t* bytes) {
  size_t code_units;
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, NULL, 0, &code_units));
  size_t null_t_bytes = (code_units + 1) * 2; // + 1 to account for null terminator
  *buffer = malloc(null_t_bytes);
  // NOTE: Cannot truncate results; causes weird test failures on windows
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, (char16_t*) *buffer, null_t_bytes, &code_units));
  *bytes = null_t_bytes - 2; // We don't care about the null terminator
  return NULL;
}
