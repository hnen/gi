
#include <string.h>
#include "xxCommon.h"

static void obj_countLines(const char * content, const char * linetype, int * out_linecount);

XXerr obj_parse(const char * filename) {
  char * content;
  size_t contentSize;
  file_read(filename, &content, &contentSize);

  int vertcount = -1, facecount = -1, normalcount = -1;
  obj_countLines(content, "v", &vertcount);
  obj_countLines(content, "vn", &facecount);
  obj_countLines(content, "f", &normalcount);
  printf("obj %s verts: %d, normals: %d, faces: %d\n", filename, vertcount, normalcount, facecount);

  return 0;
}

static void obj_countLines(const char * content, const char * linetype, int * out_linecount) {
  char buffer[128];
  int count = 0;
  while ((*content) != '\0') {
    if ((*content) != '#') {
      /* read word */
      int i = 0;
      while (*content != ' ') {
        buffer[i++] = *content;
        content++;
      }
      buffer[i] = '\0';
      if (strcmp(buffer, linetype) == 0) {
        count++;
      }
    } 
    /* skip line */
    while(*content != '\n' && *content != '\0')
      content++;
    if (*content != '\0') {
      content++;
    }
  }
  *out_linecount = count;
}



