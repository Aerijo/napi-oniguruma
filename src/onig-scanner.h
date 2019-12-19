#ifndef ONIG_SCANNER_H
#define ONIG_SCANNER_H

#include <node_api.h>
#include <stdlib.h>

#include "./common.h"
#include "./onig-reg-exp.h"
#include "./onig-search.h"
#include "./onig-string.h"

typedef struct OnigScanner {
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
} OnigScanner;

OnigScanner* onig_scanner_init(OnigRegExp** reg_exps, size_t num_reg_exps);

void onig_scanner_destroy(OnigScanner* self);

napi_value onig_scanner_find_next_match_sync(napi_env env, OnigSearchData* s);

#endif
