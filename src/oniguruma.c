#define NAPI_EXPERIMENTAL
#include <node_api.h> // https://github.com/nodejs/node/blob/master/src/node_api.h
#include <stdio.h>
#include <stdlib.h>
#include <oniguruma.h>

#include "./common.h"
#include "./onig-scanner.h"
#include "./onig-string.h"
#include "./onig-reg-exp.h"



void napi_finalize_onig_scanner(napi_env env, void* finalize_data, void* finalize_hint) {
  onig_scanner_destroy(finalize_data);
}

napi_value js_onig_scanner_constructor(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t num_args = ARRAY_LENGTH(argv);
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

void napi_finalize_onig_string(napi_env env, void* finalize_data, void* finalize_hint) {
  onig_string_destroy(finalize_data);
}

napi_value js_onig_string_constructor(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t num_args = ARRAY_LENGTH(argv);
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &num_args, argv, &_this, NULL));

  napi_value js_string = argv[0];
  NAPI_CALL(env, napi_set_named_property(env, _this, "content", js_string));

  size_t length;
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, NULL, 0, &length));
  char16_t* contents = malloc(sizeof(char16_t) * length);
  NAPI_CALL(env, napi_get_value_string_utf16(env, js_string, contents, length, &length));
  OnigString* onig_string = onig_string_init((char*) contents, length);
  NAPI_CALL(env, napi_wrap(env, _this, onig_string, napi_finalize_onig_string, NULL, NULL));

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

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  OnigEncoding encodings[2];
  encodings[0] = ONIG_ENCODING_UTF8;
  encodings[1] = ONIG_ENCODING_UTF16_LE;
  onig_initialize(encodings, ARRAY_LENGTH(encodings));

  napi_value js_onig_scanner;
  const napi_property_descriptor js_onig_scanner_properties[] = {
    {"bar", NULL, on_scanner_bar, NULL, NULL, NULL, napi_default, NULL}
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
