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

void onig_string_destroy(OnigString* self) {
  free(self->contents);
  free(self);
}

#endif
