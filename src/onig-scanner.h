#ifndef ONIG_SCANNER_H
#define ONIG_SCANNER_H

#include <stdlib.h>

#include "./onig-reg-exp.h"

typedef struct OnigScanner {
  OnigRegExp** reg_exps;
  size_t num_reg_exps;
} OnigScanner;

OnigScanner* onig_scanner_init(OnigRegExp** reg_exps, size_t num_reg_exps) {
  OnigScanner* self = malloc(sizeof(OnigScanner));
  self->reg_exps = reg_exps;
  self->num_reg_exps = num_reg_exps;
  return self;
}

void onig_scanner_destroy(OnigScanner* self) {
  for (size_t i = 0; i < self->num_reg_exps; i++) {
    onig_reg_exp_destroy(self->reg_exps[i]);
  }
  free(self->reg_exps);
  free(self);
}

#endif
