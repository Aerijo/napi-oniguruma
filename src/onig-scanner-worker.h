#ifndef ONIG_SCANNER_WORKER_H
#define ONIG_SCANNER_WORKER_H

#include <assert.h>
#include <stdio.h>

#include "./common.h"
#include "./onig-result.h"
#include "./onig-searcher.h"
#include "./onig-string.h"

typedef struct OnigSearchData {
  OnigString* source;
  size_t start_byte_offset;
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
  OnigResult* best_result;
  bool cache_results;
  bool free_string;
} OnigSearchData;

typedef enum AsyncKind {
  JS_PROMISE,
  JS_CALLBACK
} AsyncKind;

typedef struct OnigAsyncData {
  OnigSearchData* search_data;
  napi_ref _this;
  AsyncKind _return_kind;
  union {
    napi_ref _callback;
    napi_deferred _deferred;
  };
  napi_async_work _request;
} OnigAsyncData;

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

void execute_async_search_callback(napi_env env, void* _data) {
  OnigAsyncData* data = _data;
  OnigSearchData* s = data->search_data;
  s->best_result = search(s->source, s->start_byte_offset, s->reg_exps, s->num_reg_exps, s->cache_results);
}

void complete_async_search_callback(napi_env env, napi_status status, void* _data) {
  OnigAsyncData* data = _data;
  OnigSearchData* search_data = data->search_data;

  napi_value _this;
  NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, data->_this, &_this));

  napi_value callback;
  if (data->_return_kind == JS_CALLBACK) {
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, data->_callback, &callback));
  }

  if (search_data->best_result != NULL) {
    OnigResult* best = search_data->best_result;

    napi_value result;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result));

    napi_value scanner_index;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, onig_result_get_scanner_index(best), &scanner_index));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "index", scanner_index));

    if (data->_return_kind == JS_CALLBACK) {
      NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "scanner", _this));
    }

    size_t capture_count = onig_result_num_captures(best);
    napi_value captures_array;
    NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, capture_count, &captures_array));

    for (size_t i = 0; i < capture_count; i++) {
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

    if (data->_return_kind == JS_PROMISE) {
      NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, data->_deferred, result));
      data->_deferred = NULL;
    } else if (data->_return_kind == JS_CALLBACK) {
      napi_value null_result;
      NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &null_result));
      napi_value argv[2] = {null_result, result};
      NAPI_CALL_RETURN_VOID(env, napi_call_function(env, _this, callback, 2, argv, NULL));
    } else {
      assert(false);
    }
  } else {
    napi_value null_result;
    NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &null_result));

    if (data->_return_kind == JS_PROMISE) {
      NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, data->_deferred, null_result));
      data->_deferred = NULL;
    } else if (data->_return_kind == JS_CALLBACK) {
      napi_value argv[2] = {null_result, null_result};
      NAPI_CALL_RETURN_VOID(env, napi_call_function(env, _this, callback, 2, argv, NULL));
    } else {
      assert(false);
    }
  }

  if (data->_return_kind == JS_PROMISE) {
    // no promise specific cleanup; napi_resolve_... frees the deferred object
  } else if (data->_return_kind == JS_CALLBACK) {
    NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, data->_callback));
  } else {
    assert(false);
  }

  NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, data->_this));
  NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, data->_request));
  onig_search_data_destroy(search_data);
}

napi_value create_async_search(napi_env env, OnigAsyncData* data) {
  assert(data->_request == NULL);
  napi_value resource_name;
  NAPI_CALL(env, napi_create_string_utf8(env, "OnigScannerWorker", NAPI_AUTO_LENGTH, &resource_name));
  NAPI_CALL(env, napi_create_async_work(
    env,
    NULL,
    resource_name,
    execute_async_search_callback,
    complete_async_search_callback,
    (void*) data,
    &data->_request
  ));
  return NULL;
}

napi_value queue_async_search(napi_env env, OnigAsyncData* data) {
  assert(data->_request != NULL);
  NAPI_CALL(env, napi_queue_async_work(env, data->_request));
  return NULL;
}

napi_value onig_search_async_promise(napi_env env, napi_value _this, napi_deferred deferred, OnigSearchData* search_data) {
  napi_ref _this_ref;
  NAPI_CALL(env, napi_create_reference(env, _this, 1, &_this_ref));

  OnigAsyncData* async_data = malloc(sizeof(OnigAsyncData));
  async_data->search_data = search_data;
  async_data->_this = _this_ref;
  async_data->_return_kind = JS_PROMISE;
  async_data->_deferred = deferred;
  async_data->_request = NULL;

  NAPI_PROPAGATE_FAILURE(create_async_search(env, async_data));
  NAPI_PROPAGATE_FAILURE(queue_async_search(env, async_data));
  return NULL;
}

napi_value onig_search_async_callback(napi_env env, napi_value _this, napi_value cb, OnigSearchData* search_data) {
  napi_ref _this_ref, _cb_ref;
  NAPI_CALL(env, napi_create_reference(env, _this, 1, &_this_ref));
  NAPI_CALL(env, napi_create_reference(env, cb, 1, &_cb_ref));

  OnigAsyncData* async_data = malloc(sizeof(OnigAsyncData));
  async_data->search_data = search_data;
  async_data->_this = _this_ref;
  async_data->_return_kind = JS_CALLBACK;
  async_data->_callback = _cb_ref;
  async_data->_request = NULL;

  NAPI_PROPAGATE_FAILURE(create_async_search(env, async_data));
  NAPI_PROPAGATE_FAILURE(queue_async_search(env, async_data));
  return NULL;
}

#endif
