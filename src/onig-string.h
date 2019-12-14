#ifndef ONIG_STRING_H
#define ONIG_STRING_H

#include <oniguruma.h>

static int id_counter = 0;

typedef struct OnigString {
  int id;
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
