#ifndef ONIG_STRING_H
#define ONIG_STRING_H

#include <oniguruma.h>

#define ONIG_STRING_NULL_ID 0

static size_t id_counter = 1; // 0 is NULL value; no string should have ID of 0

typedef struct OnigString {
  size_t id;
  OnigEncoding encoding;

  char* contents;
  size_t length;

  bool has_multibyte_characters;
} OnigString;

OnigString* onig_string_init(char* contents, size_t length) {
  OnigString* self = malloc(sizeof(OnigString));
  self->id = ++id_counter;
  self->encoding = ONIG_ENCODING_UTF16_LE;
  self->contents = contents;
  self->length = length;
  return self;
}

void retrieve_onig_string(napi_env env, napi_value object, OnigString** output, bool* is_js_string) {
  napi_valuetype string_type;
  NAPI_CALL_RETURN_VOID(env, napi_typeof(env, object, &string_type));
  *is_js_string = string_type == napi_string;

  if (*is_js_string) {
    char* contents;
    size_t bytes;
    get_js_utf16_string(env, object, &contents, &bytes);
    *output = onig_string_init(contents, bytes);
  } else {
    void* t;
    NAPI_CALL_RETURN_VOID(env, napi_unwrap(env, object, &t));
    *output = t;
  }
}

void onig_string_destroy(OnigString* self) {
  free(self->contents);
  free(self);
}

#endif
