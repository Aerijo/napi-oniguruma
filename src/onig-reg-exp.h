#ifndef ONIG_REG_EXP_H
#define ONIG_REG_EXP_H

#include <oniguruma.h>

typedef struct OnigRegExp {
  char* source;
  OnigRegexType* regex;
  bool has_G_anchor;
  int last_search_string_unique_id;
  int last_search_position;
} OnigRegExp;

OnigRegExp* onig_reg_exp_init(const char* pattern, size_t length) {
  return NULL;
}

void onig_reg_exp_destroy(OnigRegExp* self) {

}

#endif
