
#include <string.h>
#include "xxCommon.h"

typedef void(*datagettercallback)(const char * line, void * out_data);

static void obj_countLines(const char * content, const char * linetype, int * out_linecount);
static void obj_getData(const char * content, const char * linetype, datagettercallback datagetter);

static void vertexparser(const char * line, void * out_data);

XXerr obj_parse(const char * filename) {
  char * content;
  size_t contentSize;
  file_read(filename, &content, &contentSize);
    
  int vertcount = -1, facecount = -1, normalcount = -1;
  obj_countLines(content, "v", &vertcount);
  obj_countLines(content, "vn", &facecount);
  obj_countLines(content, "f", &normalcount);
  printf("obj %s verts: %d, normals: %d, faces: %d\n", filename, vertcount, normalcount, facecount);

  //obj_getData(content, "v", &vertexparser);

  free(content);
    
  return 0;
}

static void vertexparser(const char * line, void * out_data) {
  float * fdata = (float*)out_data;
  sscanf(line, "%f %f %f", &fdata[0], &fdata[1], &fdata[2]);
}

int readnextline(const char ** ptr, char * out_cmd, char * out_content);

static void obj_getData(const char * content, const char * linetype, datagettercallback datagetter) {

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





