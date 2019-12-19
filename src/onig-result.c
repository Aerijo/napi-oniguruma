#include <stdlib.h>

#include "./common.h"
#include "./onig-result.h"

OnigResult* onig_result_init(OnigRegion* region) {
  OnigResult* self = malloc(sizeof(OnigResult));
  self->region = region;
  self->scanner_index = 0;
  return self;
}

void onig_result_destroy(OnigResult* self) {
  onig_region_free(self->region, /* free all */ 1);
  free(self);
}

size_t onig_result_location_of(OnigResult* self, size_t index) {
  int bytes = *(self->region->beg + (int) index);
  if (bytes > 0) {
    return (size_t) bytes;
  } else {
    return 0;
  }
}

size_t onig_result_get_scanner_index(OnigResult* self) {
  return self->scanner_index;
}

void onig_result_set_scanner_index(OnigResult* self, size_t index) {
  self->scanner_index = index;
}

int onig_result_num_captures(OnigResult* self) {
  return self->region->num_regs;
}

size_t onig_result_group_length(OnigResult* self, size_t index) {
  int bytes = *(self->region->end + (int) index) - *(self->region->beg + (int) index);
  if (bytes > 0) {
    return (size_t) bytes;
  } else {
    return 0;
  }
}

napi_value generate_captures(napi_env env, OnigResult best) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  napi_value scanner_index;
  NAPI_CALL(env, napi_create_uint32(env, onig_result_get_scanner_index(&best), &scanner_index));
  NAPI_CALL(env, napi_set_named_property(env, result, "index", scanner_index));

  size_t capture_count = onig_result_num_captures(&best);
  napi_value captures_array;
  NAPI_CALL(env, napi_create_array_with_length(env, capture_count, &captures_array));

  for (size_t i = 0; i < capture_count; i++) {
    size_t capture_start = onig_result_location_of(&best, i) / 2; // HACK: divide by 2 to convert byteOffset (UTF16) -> JS string index
    size_t capture_end = capture_start + onig_result_group_length(&best, i) / 2;

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
}
