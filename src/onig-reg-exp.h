#ifndef ONIG_REG_EXP_H
#define ONIG_REG_EXP_H

#include <oniguruma.h>

#include "./common.h"
#include "./onig-result.h"
#include "./onig-string.h"

typedef struct OnigRegExp {
  char* source;
  size_t source_bytes;
  regex_t* regex;
  bool has_G_anchor;
  size_t last_search_string_id;
  size_t last_search_position;
  OnigResult* last_search_result;
} OnigRegExp;

typedef struct OnigRegExpBundle {
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
  size_t references;
} OnigRegExpBundle;

bool has_G_anchor(const char* pattern, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    if (pattern[i] == '\\' && ++i < length) {
      if (pattern[i] == 'G') {
        return true;
      }
    }
  }
  return false;
}

OnigRegExp* onig_reg_exp_init(char* pattern, size_t bytes, napi_env env) {
  OnigRegExp* self = malloc(sizeof(OnigRegExp));
  self->source = pattern;
  self->source_bytes = bytes;

  OnigErrorInfo error;
  int status = onig_new(
    &self->regex,
    (const UChar*) pattern,
    (const UChar*) pattern + bytes,
    ONIG_OPTION_CAPTURE_GROUP,
    ONIG_ENCODING_UTF16_LE,
    ONIG_SYNTAX_DEFAULT,
    &error
  );

  if (status != ONIG_NORMAL) {
    char error_string[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar*) error_string, status, &error);
    napi_throw_error(env, NULL, error_string);
  }

  self->has_G_anchor = has_G_anchor(pattern, bytes);
  self->last_search_string_id = ONIG_STRING_NULL_ID;
  self->last_search_position = 0;
  self->last_search_result = NULL;

  return self;
}

void onig_reg_exp_destroy(OnigRegExp* self) {
  if (self->regex != NULL) {
    onig_free(self->regex);
  }
  if (self->last_search_result != NULL) {
    onig_result_destroy(self->last_search_result);
  }
  free(self->source);
  free(self);
}

OnigResult* onig_reg_exp_search(OnigRegExp* self, char* source, size_t length, size_t start) {
  if (self->regex == NULL) {
    return NULL;
  }

  OnigRegion* region = onig_region_new();
  int status = onig_search(
    self->regex,
    (UChar*) source,
    (UChar*) source + length,
    (UChar*) source + start,
    (UChar*) source + length,
    region,
    ONIG_OPTION_NONE
  );

  if (status > ONIG_MISMATCH) {
    return onig_result_init(region);
  }

  onig_region_free(region, 1);
  if (status == ONIG_MISMATCH) {
    return NULL;
  } else {
    // error
    return NULL;
  }
}

OnigResult* onig_reg_exp_search_onig_string(OnigRegExp* self, OnigString* source, size_t start, bool cache) {
  if (!cache || self->has_G_anchor) {
    return onig_reg_exp_search(self, source->contents, source->length, start);
  }

  if (self->last_search_string_id == source->id && self->last_search_position <= start) {
    if (self->last_search_result == NULL || onig_result_location_of(self->last_search_result, 0) >= start) {
      return self->last_search_result;
    }
  }

  if (self->last_search_result != NULL) {
    onig_result_destroy(self->last_search_result);
  }

  self->last_search_string_id = source->id;
  self->last_search_position = start;
  self->last_search_result = onig_reg_exp_search(self, source->contents, source->length, start);
  return self->last_search_result;
}

#endif
