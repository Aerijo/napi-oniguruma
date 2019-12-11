#define NAPI_EXPERIMENTAL
#include <node_api.h> // https://github.com/nodejs/node/blob/master/src/node_api.h
#include <stdio.h>
#include <stdlib.h>

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
            ? "empty error message"                               \
            : error_info->error_message;                          \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)

typedef struct Foo {
  int foo;
} Foo;

void napi_finalize_onig_scanner(napi_env env, void* finalize_data, void* finalize_hint) {
  printf("Freeing with result %d\n", ((Foo*) finalize_data)->foo);
  free(finalize_data);
}

napi_value napi_onig_scanner_constructor(napi_env env, napi_callback_info info) {
  size_t num_args = 1;
  napi_value argv[1];

  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, &num_args, argv, &_this, NULL));

  int result;
  NAPI_CALL(env, napi_get_value_int32(env, argv[0], &result));

  NAPI_CALL(env, napi_set_named_property(env, _this, "foo", argv[0]));

  Foo *foo = malloc(sizeof(Foo));
  foo->foo = result;

  NAPI_CALL(env, napi_wrap(env, _this, foo, napi_finalize_onig_scanner, NULL, NULL));

  return _this;
}

napi_value on_scanner_bar(napi_env env, napi_callback_info info) {
  printf("Calling bar...\n");
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));

  printf("Got this...\n");
  Foo *foo;
  NAPI_CALL(env, napi_unwrap(env, _this, &foo));

  printf("Called bar with %d\n", foo->foo);

  napi_value result;
  NAPI_CALL(env, napi_create_int32(env, 12, &result));
  return result;
}

NAPI_MODULE_INIT() {
  /**
   * The following are parameters to this method defined in the macro:
   * napi_env env
   * napi_value exports
   */

  napi_value foo_value;
  NAPI_CALL(env, napi_create_int32(env, 123, &foo_value));
  const napi_property_descriptor properties[] = {
    {"foo", NULL, NULL, NULL, NULL, foo_value, napi_enumerable, NULL},
    {"bar", NULL, on_scanner_bar, NULL, NULL, NULL, napi_enumerable, NULL}
  };

  napi_value js_scanner;
  NAPI_CALL(env, napi_define_class(
    env,
    "OnigScanner",
    NAPI_AUTO_LENGTH,
    napi_onig_scanner_constructor,
    NULL,
    sizeof(properties) / sizeof(*properties),
    properties,
    &js_scanner
  ));

  NAPI_CALL(env, napi_set_named_property(env, exports, "OnigScanner", js_scanner));

  return exports;
}
