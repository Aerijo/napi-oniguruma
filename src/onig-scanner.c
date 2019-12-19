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
    napi_value result;
    NAPI_CALL(env, napi_create_object(env, &result));

    napi_value scanner_index;
    NAPI_CALL(env, napi_create_uint32(env, onig_result_get_scanner_index(best_result), &scanner_index));
    NAPI_CALL(env, napi_set_named_property(env, result, "index", scanner_index));

    size_t result_count = onig_result_num_captures(best_result);

    napi_value captures_array;
    NAPI_CALL(env, napi_create_array_with_length(env, result_count, &captures_array));
    for (size_t i = 0; i < result_count; i++) {
      size_t capture_start = onig_result_location_of(best_result, i) / 2; // HACK: divide by 2 to convert byteOffset (UTF16) -> JS string index
      size_t capture_end = capture_start + onig_result_group_length(best_result, i) / 2;

      napi_value capture;
      NAPI_CALL(env, napi_create_object(env, &capture));

      napi_value t;
      NAPI_CALL(env, napi_create_uint32(env, capture_start, &t));
      NAPI_CALL(env, napi_set_named_property(env, capture, "start", t));
      NAPI_CALL(env, napi_create_uint32(env, capture_end, &t));
      NAPI_CALL(env, napi_set_named_property(env, capture, "end", t));
      NAPI_CALL(env, napi_create_uint32(env, capture_end - capture_start, &t));
      NAPI_CALL(env, napi_set_named_property(env, capture, "length", t));
      NAPI_CALL(env, napi_set_element(env, captures_array, i, capture));
    }

    NAPI_CALL(env, napi_set_named_property(env, result, "captureIndices", captures_array));
    return result;
  } else {
    napi_value null_result;
    NAPI_CALL(env, napi_get_null(env, &null_result));
    return null_result;
  }
}
