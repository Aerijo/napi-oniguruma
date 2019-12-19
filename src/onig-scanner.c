#include <oniguruma.h>

#include "./onig-scanner.h"

OnigScanner* onig_scanner_init(OnigRegExp** reg_exps, size_t num_reg_exps) {
  OnigScanner* self = malloc(sizeof(OnigScanner));
  self->reg_exps = reg_exps;
  self->num_reg_exps = num_reg_exps;
  return self;
}

void onig_scanner_destroy(OnigScanner* self) {
  for (size_t i = 0; i < self->num_reg_exps; i++) {
    onig_reg_exp_destroy(self->reg_exps[i]);
  }
  free(self->reg_exps);
  free(self);
}

napi_value onig_scanner_find_next_match_sync(napi_env env, OnigSearchData* s) {
  OnigResult* best_result = search(s->source, s->start_byte_offset, s->reg_exps, s->num_reg_exps, s->cache_results);

  if (best_result != NULL) {
    return generate_captures(env, best_result);
  } else {
    napi_value null_result;
    NAPI_CALL(env, napi_get_null(env, &null_result));
    return null_result;
  }
}
