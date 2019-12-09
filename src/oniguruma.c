#include <node_api.h>

NAPI_MODULE_INIT() {
  napi_value answer;
  napi_status status;

  status = napi_create_int64(env, 42, &answer);
  if (status != napi_ok) return NULL;

  status = napi_set_named_property(env, exports, "answer", answer);
  if (status != napi_ok) return NULL;

  return exports;
}
