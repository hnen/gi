
#ifndef __XXPARSEOBJ_H
#define __XXPARSEOBJ_H

#include "common.h"
#include "vec.h"

typedef struct {
  vec3 * v_pos;
  int v_count;

  int * f_ind;
  int f_count;

  vec3 * f_nrm;
  int f_nrm_count;
} XXobj;

XXerr obj_parse(const char * filename, XXobj * out_obj);
void obj_printf(XXobj * obj);
void obj_release(XXobj * obj);

#endif

