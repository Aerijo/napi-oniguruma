#include <stdlib.h>

#include "./onig-search.h"

OnigSearchData* onig_search_data_init(OnigString* source, size_t start_byte, OnigRegExp** reg_exps, size_t num_reg_exps, bool cache_results, bool free_string) {
  OnigSearchData* self = malloc(sizeof(OnigSearchData));
  self->source = source;
  self->start_byte_offset = start_byte;
  self->reg_exps = reg_exps;
  self->num_reg_exps = num_reg_exps;
  self->best_result = NULL;
  self->cache_results = cache_results;
  self->free_string = free_string;
  return self;
}

void onig_search_data_destroy(OnigSearchData* self) {
  if (self->free_string) {
    onig_string_destroy(self->source);
    self->source = NULL;
  }

  // reg_exps always belong to scanner

  // If caching results, the regex owns the result
  if (!self->cache_results && self->best_result != NULL) {
    onig_result_destroy(self->best_result);
    self->best_result = NULL;
  }

  free(self);
}

OnigResult* search(OnigString* source, size_t offset, OnigRegExp** reg_exps, size_t num_reg_exps, bool cache) {
  size_t best_location = 0;
  OnigResult* best_result = NULL;

  for (size_t i = 0; i < num_reg_exps; i++) {
    OnigResult* result = onig_reg_exp_search_onig_string(reg_exps[i], source, offset, cache);
    if (result != NULL) {
      if (onig_result_num_captures(result) > 0) {
        size_t location = onig_result_location_of(result, 0);
        if (best_result == NULL || location < best_location) {
          if (!cache && best_result != NULL) {
            onig_result_destroy(best_result);
          }
          best_location = location;
          best_result = result;
          onig_result_set_scanner_index(best_result, i);
        } else if (!cache) {
          onig_result_destroy(result);
        }

        if (location == offset) {
          break;
        }
      } else if (!cache) {
        onig_result_destroy(result);
      }
    }
  }

  return best_result;
}
