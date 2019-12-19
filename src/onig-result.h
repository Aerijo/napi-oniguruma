#ifndef ONIG_RESULT_H
#define ONIG_RESULT_H

#include <node_api.h>
#include <oniguruma.h>
#include <stddef.h>

typedef struct OnigResult {
  OnigRegion* region;
  size_t scanner_index;
} OnigResult;

OnigResult* onig_result_init(OnigRegion* region);

void onig_result_destroy(OnigResult* self);

size_t onig_result_location_of(OnigResult* self, size_t index);

size_t onig_result_get_scanner_index(OnigResult* self);

void onig_result_set_scanner_index(OnigResult* self, size_t index);

int onig_result_num_captures(OnigResult* self);

size_t onig_result_group_length(OnigResult* self, size_t index);

napi_value generate_captures(napi_env env, OnigResult best);

#endif
