#ifndef ONIG_SCANNER_H
#define ONIG_SCANNER_H

#include "./onig-reg-exp.h"

typedef struct OnigScanner {
  size_t num_expressions;
} OnigScanner;

OnigScanner* onig_scanner_init(OnigRegExp* reg_exp[], size_t num_expressions) {
  OnigScanner* self = malloc(sizeof(OnigScanner));

  self->num_expressions = num_expressions;

  return self;
}

void onig_scanner_destroy(OnigScanner* self) {
  free(self);
}

#endif
