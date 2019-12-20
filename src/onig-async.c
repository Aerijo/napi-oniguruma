#include <assert.h>
#include <node_api.h>

#include "./common.h"
#include "./onig-async.h"

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

    napi_value result = generate_captures(env, best);

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
  free(data);
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
