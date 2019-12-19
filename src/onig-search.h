#ifndef ONIG_SEARCH_H
#define ONIG_SEARCH_H

#include "./onig-string.h"
#include "./onig-reg-exp.h"

typedef struct OnigSearchData {
  OnigString* source;
  size_t start_byte_offset;
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
  OnigResult* best_result;
  bool cache_results;
  bool free_string;
} OnigSearchData;

OnigSearchData* onig_search_data_init(OnigString* source, size_t start_byte, OnigRegExp** reg_exps, size_t num_reg_exps, bool cache_results, bool free_string);

void onig_search_data_destroy(OnigSearchData* self);

OnigResult* search(OnigString* source, size_t offset, OnigRegExp** reg_exps, size_t num_reg_exps, bool cache);

#endif
