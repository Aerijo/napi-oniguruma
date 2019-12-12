#define NAPI_EXPERIMENTAL
#include <node_api.h> // https://github.com/nodejs/node/blob/master/src/node_api.h
#include <stdio.h>
#include <stdlib.h>

#include "./onig-scanner.h"
#include "./onig-reg-exp.h"

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

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

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

void napi_finalize_onig_scanner(napi_env env, void* finalize_data, void* finalize_hint) {
  onig_scanner_destroy(finalize_data);
}

napi_value napi_onig_scanner_constructor(napi_env env, napi_callback_info info) {
  size_t num_args = 1;
  napi_value argv[1];

  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &num_args, argv, &_this, NULL));

  napi_value patterns_array = argv[0];
  uint32_t num_patterns;
  NAPI_CALL(env, napi_get_array_length(env, patterns_array, &num_patterns));

  OnigRegExp** reg_exps = malloc(sizeof(OnigRegExp*) * num_patterns);

  for (size_t i = 0; i < num_patterns; i++) {
    napi_value js_pattern;
    NAPI_CALL(env, napi_get_element(env, patterns_array, i, &js_pattern));

    size_t pattern_length;
    NAPI_CALL(env, napi_get_value_string_utf8(env, js_pattern, NULL, 0, &pattern_length));

    char* pattern_buffer = malloc(sizeof(char) * pattern_length);
    NAPI_CALL(env, napi_get_value_string_utf8(env, js_pattern, pattern_buffer, pattern_length, &pattern_length));

    reg_exps[i] = onig_reg_exp_init(pattern_buffer, pattern_length);
  }

  OnigScanner* scanner = onig_scanner_init(reg_exps, num_patterns);
  NAPI_CALL(env, napi_wrap(env, _this, scanner, napi_finalize_onig_scanner, NULL, NULL));

  return _this;
}

napi_value on_scanner_bar(napi_env env, napi_callback_info info) {
  printf("Calling bar...\n");
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));

  void *data;
  NAPI_CALL(env, napi_unwrap(env, _this, &data));

  OnigScanner *scanner = data;
  printf("called bar on scanner with %ld expressions\n", scanner->num_expressions);

  napi_value result;
  NAPI_CALL(env, napi_create_int32(env, 12, &result));
  return result;
}

NAPI_MODULE_INIT() {
  /**
   * The following are parameters to this method defined in the macro:
   * napi_env env
   * napi_value exports
   */

  OnigEncoding encodings[1];
  encodings[0] = ONIG_ENCODING_UTF8;

  onig_initialize(encodings, ARRAY_LENGTH(encodings));

  napi_value foo_value;
  NAPI_CALL(env, napi_create_int32(env, 123, &foo_value));
  const napi_property_descriptor properties[] = {
    {"foo", NULL, NULL, NULL, NULL, foo_value, napi_enumerable, NULL},
    {"bar", NULL, on_scanner_bar, NULL, NULL, NULL, napi_enumerable, NULL}
  };

  napi_value js_scanner;
  NAPI_CALL(env, napi_define_class(
    env,
    "OnigScanner",
    NAPI_AUTO_LENGTH,
    napi_onig_scanner_constructor,
    NULL,
    ARRAY_LENGTH(properties),
    properties,
    &js_scanner
  ));

  NAPI_CALL(env, napi_set_named_property(env, exports, "OnigScanner", js_scanner));

  return exports;
}
