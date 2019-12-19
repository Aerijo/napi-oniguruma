#ifndef ONIG_REG_EXP_H
#define ONIG_REG_EXP_H

#include <oniguruma.h>

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

OnigRegExp* onig_reg_exp_init(char* pattern, size_t bytes, napi_env env);

void onig_reg_exp_destroy(OnigRegExp* self);

bool has_G_anchor(const char* pattern, size_t length);

OnigResult* onig_reg_exp_search(OnigRegExp* self, char* source, size_t length, size_t start);

OnigResult* onig_reg_exp_search_onig_string(OnigRegExp* self, OnigString* source, size_t start, bool cache);

#endif
