#ifndef ONIG_STRING_H
#define ONIG_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include <oniguruma.h>
#include <node_api.h>

#define ONIG_STRING_NULL_ID 0

typedef struct OnigString {
  size_t id;
  OnigEncoding encoding;
  char* contents;
  size_t length;
  bool has_multibyte_characters;
} OnigString;

OnigString* onig_string_init(char* contents, size_t length);

void onig_string_destroy(OnigString* self);

void retrieve_onig_string(napi_env env, napi_value object, OnigString** output, bool* is_js_string);

#endif
