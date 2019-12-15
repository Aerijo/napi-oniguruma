#ifndef ONIG_SEARCHER_H
#define ONIG_SEARCHER_H

#include "./onig-string.h"
#include "./onig-reg-exp.h"

OnigResult* search(OnigString* source, size_t offset, OnigRegExp** reg_exps, size_t num_reg_exps) {
  size_t best_location = 0;
  OnigResult* best_result = malloc(sizeof(OnigResult));
  OnigResult* current_result = malloc(sizeof(OnigResult));

  for (size_t i = 0; i < num_reg_exps; i++) {
    onig_reg_exp_search_onig_string(reg_exps[i], source, offset, current_result);
  }

}

#endif
