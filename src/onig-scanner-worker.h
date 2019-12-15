#ifndef ONIG_SCANNER_WORKER_H
#define ONIG_SCANNER_WORKER_H

#include <stdio.h>

#include "./common.h"
#include "./onig-result.h"
#include "./onig-searcher.h"
#include "./onig-string.h"

typedef struct OnigScannerData {
  OnigString* source;
  size_t offset;
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
  OnigResult* best_result;
  napi_ref _this;
  napi_ref _callback;
  napi_async_work _request;
} OnigScannerData;

OnigScannerData* onig_scanner_data_construct(OnigString* source, size_t offset, OnigRegExp** reg_exps, size_t num_reg_exps, napi_ref cb, napi_ref _this) {
  OnigScannerData* self = malloc(sizeof(OnigScannerData));
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

void onig_scanner_data_destroy(OnigScannerData* self) {
  onig_string_destroy(self->source);
  // regexes belong to scanner
  free(self->best_result);
  free(self);
}

void execute_callback(napi_env env, void* _data) {
  OnigScannerData* data = _data;
  data->best_result = search(data->source, data->offset, data->reg_exps, data->num_reg_exps);
}

void complete_callback(napi_env env, napi_status status, void* _data) {
  OnigScannerData* data = _data;

  napi_value _this;
  NAPI_CALL_NO_RET(env, napi_get_reference_value(env, data->_this, &_this));
  napi_value callback;
  NAPI_CALL_NO_RET(env, napi_get_reference_value(env, data->_callback, &callback));

  if (data->best_result != NULL) {

  } else {
    napi_value null_result;
    NAPI_CALL_NO_RET(env, napi_get_null(env, &null_result));

    napi_value argv[2] = {null_result, null_result};

    NAPI_CALL_NO_RET(env, napi_call_function(env, _this, callback, 2, argv, NULL));
  }

  NAPI_CALL_NO_RET(env, napi_delete_reference(env, data->_this));
  NAPI_CALL_NO_RET(env, napi_delete_reference(env, data->_callback));
  NAPI_CALL_NO_RET(env, napi_delete_async_work(env, data->_request));
  onig_scanner_data_destroy(data);
}

napi_value submit_async_search(napi_env env, OnigString* source, size_t offset, OnigRegExp* reg_exps[], size_t num_reg_exps, napi_ref cb, napi_ref _this) {
  napi_value resource_name;
  NAPI_CALL(env, napi_create_string_utf8(env, "OnigScannerWorker", NAPI_AUTO_LENGTH, &resource_name));

  OnigScannerData* data = onig_scanner_data_construct(source, offset, reg_exps, num_reg_exps, cb, _this);

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
