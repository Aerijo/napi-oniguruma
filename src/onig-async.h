#ifndef ONIG_ASYNC_H
#define ONIG_ASYNC_H

#include "./onig-search.h"

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

napi_value create_async_search(napi_env env, OnigAsyncData* data);

napi_value queue_async_search(napi_env env, OnigAsyncData* data);

void execute_async_search_callback(napi_env env, void* _data);

void complete_async_search_callback(napi_env env, napi_status status, void* _data);

napi_value onig_search_async_promise(napi_env env, napi_value _this, napi_deferred deferred, OnigSearchData* search_data);

napi_value onig_search_async_callback(napi_env env, napi_value _this, napi_value cb, OnigSearchData* search_data);

#endif
