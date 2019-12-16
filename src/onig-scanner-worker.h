#ifndef ONIG_SCANNER_WORKER_H
#define ONIG_SCANNER_WORKER_H

#include <stdio.h>

#include "./common.h"
#include "./onig-result.h"
#include "./onig-searcher.h"
#include "./onig-string.h"

typedef struct OnigScannerWorker {
  OnigString* source;
  size_t offset;
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
  OnigResult* best_result;
  napi_ref _this;
  napi_ref _callback;
  napi_async_work _request;
} OnigScannerWorker;

OnigScannerWorker* onig_scanner_worker_construct(OnigString* source, size_t offset, OnigRegExp** reg_exps, size_t num_reg_exps, napi_ref cb, napi_ref _this) {
  OnigScannerWorker* self = malloc(sizeof(OnigScannerWorker));
  self->source = source;
  self->offset = offset;
  self->reg_exps = reg_exps;
  self->num_reg_exps = num_reg_exps;
  self->best_result = NULL;
  self->_this = _this;
  self->_callback = cb;
  self->_request = NULL;
  return self;
}

void onig_scanner_worker_destroy(OnigScannerWorker* self) {
  onig_string_destroy(self->source);
  // reg_exps belong to scanner
  if (self->best_result != NULL) { // this is async work, so we own the result
    onig_result_destroy(self->best_result);
  }
  free(self);
}

void execute_callback(napi_env env, void* _data) {
  OnigScannerWorker* data = _data;
  data->best_result = search(data->source, data->offset, data->reg_exps, data->num_reg_exps, /* cache */ false);
}

void complete_callback(napi_env env, napi_status status, void* _data) {
  OnigScannerWorker* data = _data;

  napi_value _this;
  NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, data->_this, &_this));
  napi_value callback;
  NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, data->_callback, &callback));

  if (data->best_result != NULL) {
    OnigResult* best = data->best_result;

    napi_value result;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result));

    napi_value scanner_index;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, onig_result_get_scanner_index(best), &scanner_index));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "index", scanner_index));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "scanner", _this));

    size_t result_count = onig_result_num_captures(best);

    napi_value captures_array;
    NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, result_count, &captures_array));
    for (size_t i = 0; i < result_count; i++) {
      size_t capture_start = onig_result_location_of(best, i) / 2; // HACK: divide by 2 to convert byteOffset (UTF16) -> JS string index
      size_t capture_end = capture_start + onig_result_group_length(best, i) / 2;

      napi_value capture;
      NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &capture));

      napi_value t;
      NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, capture_start, &t));
      NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, capture, "start", t));
      NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, capture_end, &t));
      NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, capture, "end", t));
      NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, capture_end - capture_start, &t));
      NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, capture, "length", t));

      NAPI_CALL_RETURN_VOID(env, napi_set_element(env, captures_array, i, capture));
    }

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "captureIndices", captures_array));
    napi_value null_result;
    NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &null_result));
    napi_value argv[2] = {null_result, result};
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, _this, callback, 2, argv, NULL));
  } else {
    napi_value null_result;
    NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &null_result));
    napi_value argv[2] = {null_result, null_result};
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, _this, callback, 2, argv, NULL));
  }

  NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, data->_this));
  NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, data->_callback));
  NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, data->_request));
  onig_scanner_worker_destroy(data);
}

napi_value submit_async_search(napi_env env, OnigString* source, size_t offset, OnigRegExp* reg_exps[], size_t num_reg_exps, napi_ref cb, napi_ref _this) {
  napi_value resource_name;
  NAPI_CALL(env, napi_create_string_utf8(env, "OnigScannerWorker", NAPI_AUTO_LENGTH, &resource_name));

  OnigScannerWorker* data = onig_scanner_worker_construct(source, offset, reg_exps, num_reg_exps, cb, _this);
  NAPI_CALL(env, napi_create_async_work(
    env,
    NULL,
    resource_name,
    execute_callback,
    complete_callback,
    (void*)data,
    &data->_request
  ));
  NAPI_CALL(env, napi_queue_async_work(env, data->_request));
  return NULL;
}

#endif
