#ifndef ONIG_RESULT_H
#define ONIG_RESULT_H

typedef struct OnigResult {
  OnigRegion* region;
  size_t index;
} OnigResult;

OnigResult* onig_result_init(OnigRegion* region) {
  OnigResult* self = malloc(sizeof(OnigResult));
  self->region = region;
  self->index = 0;
  return self;
}

void onig_result_destroy(OnigResult* self) {
  onig_region_free(self->region, /* free all */ 1);
  free(self);
}

size_t onig_result_location_of(OnigResult* self, size_t index) {
  int bytes = *(self->region->beg + (int) index);
  if (bytes > 0) {
    return (size_t) bytes;
  } else {
    return 0;
  }
}

int onig_result_num_groups(OnigResult* self) {
  return self->region->num_regs;
}

size_t onig_result_group_length(OnigResult* self, size_t index) {
  int bytes = *(self->region->end + (int) index) - *(self->region->beg + (int) index);
  if (bytes > 0) {
    return (size_t) bytes;
  } else {
    return 0;
  }
}

#endif
