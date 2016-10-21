
#include <string.h>
#include "common.h"
#include "obj.h"

typedef void(*datagettercallback)(const char * line, XXobj * tgt_struct);

static void obj_countLines(const char * content, const char * linetype, int * out_linecount);
static void obj_getData(const char * content, const char * linetype, XXobj * tgt_struct, datagettercallback datagetter);

static void vertexparser(const char * line, XXobj * target_struct);
static void normalparser(const char * line, XXobj * target_struct);
static void faceparser(const char * line, XXobj * target_struct);

XXerr obj_parse(const char * filename, XXobj * out_obj) {
  char * content;
  size_t contentSize;
  file_read(filename, &content, &contentSize);
    
  /* count */
  int vertcount = -1, facecount = -1, normalcount = -1;
  obj_countLines(content, "v", &vertcount);
  obj_countLines(content, "vn", &facecount);
  obj_countLines(content, "f", &normalcount);
  printf("obj %s verts: %d, normals: %d, faces: %d\n", filename, vertcount, normalcount, facecount);

  /* init out_obj */
  memset(out_obj, 0, sizeof(XXobj));
  out_obj->v_pos = (vec3*)malloc(vertcount * sizeof(vec3));
  out_obj->f_nrm = (vec3*)malloc(facecount * sizeof(vec3));
  out_obj->f_ind = (int*)malloc(facecount * 6 * sizeof(vec3));

  /* read data */
  obj_getData(content, "v", out_obj, &vertexparser);
  obj_getData(content, "vn", out_obj, &normalparser);
  obj_getData(content, "f", out_obj, &faceparser);

  xxfree(content);
  return 0;
}

static void vertexparser(const char * line, XXobj * target_struct) {
  int i = target_struct->v_count;
  target_struct->v_count++;
  vec3 * v = &target_struct->v_pos[i];
  sscanf(line, "%f %f %f", &v->x, &v->y, &v->z);
}

static void normalparser(const char * line, XXobj * target_struct) {
  int i = target_struct->f_nrm_count;
  target_struct->f_nrm_count++;
  vec3 * v = &target_struct->f_nrm[i];
  sscanf(line, "%f %f %f", &v->x, &v->y, &v->z);
}

static void faceparser(const char * line, XXobj * target_struct) {
  int i = target_struct->f_count * 3;
  target_struct->f_count+=2;
  int * f = &target_struct->f_ind[i];
  int n0, n1, n2, n3, nn;
  sscanf(line, "%d//%d %d//%d %d//%d %d//%d", &n0, &nn, &n1, &nn, &n2, &nn, &n3, &nn);
  f[0] = n0; f[1] = n1; f[2] = n2;
  f[3] = n2; f[4] = n3; f[5] = n0;
}

int readnextline(const char ** ptr, char * out_cmd, char * out_content);

static void obj_getData(const char * content, const char * linetype, XXobj * tgt_struct, datagettercallback datagetter) {
  char buffer[128];
  char cmd[16];
  while(readnextline(&content, cmd, buffer)) {
    if (strcmp(linetype, cmd) == 0) {
      datagetter(buffer, tgt_struct);
    }
  }
}


static void obj_countLines(const char * content, const char * linetype, int * out_linecount) {
  char buffer[128];
  char cmd[16];
  int count = 0;
  while(readnextline(&content, cmd, buffer)) {
    if (strcmp(linetype, cmd) == 0) {
      count++;
    }
  }
  *out_linecount = count;
}

int readnextline(const char ** ptr, char * out_cmd, char * out_content) {
  const char * content = *ptr;
  if ((*content) == '\0') {
    return 0;
  }
  while ((*content) != '\0') {
    if ((*content) != '#') {
      while (*content != ' ') {
        *(out_cmd++) = *(content++);
      }
      *out_cmd = 0;
      while(*(++content) == ' ');
      while (*content != '\n' && *content != '\0') {
        *(out_content++) = *(content++);
      }
      *out_content = 0;
      while((*content) == '\n')
        content++;
      *ptr = content;
      return 1;
    }
    /* skip line */
    while(*content != '\n' && *content != '\0')
      content++;
    if (*content != '\0') {
      content++;
    }
  }
  *ptr = content;
  return 0;
}


void obj_printf(XXobj * obj) {
  printf("verts: ");
  for(int i = 0; i < obj->v_count; i++) {
    printf("(%.2f, %.2f, %.2f) ", obj->v_pos[i].x, obj->v_pos[i].y, obj->v_pos[i].z);
  }
  printf("\nnorms: ");
  for(int i = 0; i < obj->f_nrm_count; i++) {
    printf("(%.2f, %.2f, %.2f) ", obj->f_nrm[i].x, obj->f_nrm[i].y, obj->f_nrm[i].z);
  }
  printf("\nfaces: ");
  for(int i = 0; i < obj->f_count; i++) {
    printf("(%d-%d-%d) ", obj->f_ind[i*3+0], obj->f_ind[i*3+1], obj->f_ind[i*3+2]);
  }
  printf("\n");
}



