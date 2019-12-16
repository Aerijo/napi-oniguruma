#ifndef ONIG_SEARCHER_H
#define ONIG_SEARCHER_H

#include "./onig-string.h"
#include "./onig-reg-exp.h"

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

#endif
