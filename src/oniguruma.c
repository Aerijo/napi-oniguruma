#define NAPI_EXPERIMENTAL
#include <node_api.h> // https://github.com/nodejs/node/blob/master/src/node_api.h
#include <stdio.h>
#include <stdlib.h>
#include <oniguruma.h>

#include "./common.h"
#include "./onig-reg-exp.h"
#include "./onig-scanner.h"
#include "./onig-scanner-worker.h"
#include "./onig-string.h"

#define GET_CALL_CONTEXT(env, info, num_args) \
  napi_value argv[num_args]; \
  size_t argc = num_args; \
  napi_value _this; \
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &_this, NULL));

void js_finalize_onig_scanner(napi_env env, void* finalize_data, void* finalize_hint) {
  onig_scanner_destroy(finalize_data);
}

napi_value js_onig_scanner_constructor(napi_env env, napi_callback_info info) {
  GET_CALL_CONTEXT(env, info, 1);
  napi_value patterns_array = argv[0];
  uint32_t num_patterns;
  NAPI_CALL(env, napi_get_array_length(env, patterns_array, &num_patterns));

  OnigRegExp** reg_exps = malloc(sizeof(OnigRegExp*) * num_patterns);

  for (size_t i = 0; i < num_patterns; i++) {
    napi_value js_pattern;
    NAPI_CALL(env, napi_get_element(env, patterns_array, i, &js_pattern));
    char* pattern_buffer;
    size_t pattern_bytes;
    get_js_utf16_string(env, js_pattern, &pattern_buffer, &pattern_bytes);
    reg_exps[i] = onig_reg_exp_init(pattern_buffer, pattern_bytes, env);
  }

  OnigScanner* scanner = onig_scanner_init(reg_exps, num_patterns);
  NAPI_CALL(env, napi_wrap(env, _this, scanner, js_finalize_onig_scanner, NULL, NULL));

  return _this;
}

void js_finalize_onig_string(napi_env env, void* finalize_data, void* finalize_hint) {
  onig_string_destroy(finalize_data);
}

napi_value js_onig_string_constructor(napi_env env, napi_callback_info info) {
  GET_CALL_CONTEXT(env, info, 1);

  napi_value js_string = argv[0];
  NAPI_CALL(env, napi_set_named_property(env, _this, "content", js_string));

  char* string_buffer;
  size_t string_bytes;
  get_js_utf16_string(env, js_string, &string_buffer, &string_bytes);

  OnigString* onig_string = onig_string_init(string_buffer, string_bytes);
  NAPI_CALL(env, napi_wrap(env, _this, onig_string, js_finalize_onig_string, NULL, NULL));

  return _this;
}

napi_value js_onig_scanner_find_next_match(napi_env env, napi_callback_info info) {
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));
  void* data;
  NAPI_CALL(env, napi_unwrap(env, _this, &data));
  // OnigScanner* scanner = data;

  napi_value result;
  NAPI_CALL(env, napi_create_int32(env, 12, &result));
  return result;
}

napi_value js_onig_scanner_find_next_match_cb(napi_env env, napi_callback_info info) {
  GET_CALL_CONTEXT(env, info, 3);
  void* data;
  NAPI_CALL(env, napi_unwrap(env, _this, &data));
  OnigScanner* scanner = data;

  size_t start_position = 0;
  napi_value cb;

  if (argc <= 1 || argc >= 4) {
    napi_throw_error(env, NULL, "Expected OnigScanner#findNextMatchCb to have 2 or 3 arguments");
    return NULL;
  } else if (argc == 2) {
    // (string, callback)
    cb = argv[1];
  } else {
    // (string, startPosition, callback)
    int32_t start;
    NAPI_CALL(env, napi_get_value_int32(env, argv[1], &start));
    if (start >= 0) {
      start_position = start;
    }
    cb = argv[2];
  }

  napi_ref this_ref;
  NAPI_CALL(env, napi_create_reference(env, _this, 1, &this_ref));

  napi_ref cb_ref;
  NAPI_CALL(env, napi_create_reference(env, cb, 1, &cb_ref));

  char* contents;
  size_t length;
  get_js_utf16_string(env, argv[0], &contents, &length);
  OnigString* onig_string = onig_string_init((char*) contents, length);

  submit_async_search(env, onig_string, start_position, scanner->reg_exps, scanner->num_reg_exps, cb_ref, this_ref);
  return NULL;
}

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  OnigEncoding encodings[2];
  encodings[0] = ONIG_ENCODING_UTF8;
  encodings[1] = ONIG_ENCODING_UTF16_LE;
  onig_initialize(encodings, ARRAY_LENGTH(encodings));

  napi_value js_onig_scanner;
  const napi_property_descriptor js_onig_scanner_properties[] = {
    DECLARE_NAPI_METHOD("findNextMatch", js_onig_scanner_find_next_match),
    DECLARE_NAPI_METHOD("findNextMatchCb", js_onig_scanner_find_next_match_cb),
    DECLARE_NAPI_METHOD("findNextMatchSync", js_onig_scanner_find_next_match)
  };
  NAPI_CALL(env, napi_define_class(
    env,
    "OnigScanner",
    NAPI_AUTO_LENGTH,
    js_onig_scanner_constructor,
    NULL,
    ARRAY_LENGTH(js_onig_scanner_properties),
    js_onig_scanner_properties,
    &js_onig_scanner
  ));
  NAPI_CALL(env, napi_set_named_property(env, exports, "OnigScanner", js_onig_scanner));

  napi_value js_onig_string;
  NAPI_CALL(env, napi_define_class(
    env,
    "OnigString",
    NAPI_AUTO_LENGTH,
    js_onig_string_constructor,
    NULL,
    0,
    NULL,
    &js_onig_string
  ));
  NAPI_CALL(env, napi_set_named_property(env, exports, "OnigString", js_onig_string));

  return exports;
}
