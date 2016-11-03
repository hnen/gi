
void glsl_preproc_process(char * src, char ** out_src) {
  int p = 0;

  while(src[p] != 0) {
    if (src[p] == '#') {
      p++;
      // read word
      char w[1024];
      int w_p = 0;
      while(src[p] != ' ') {
        w[w_p++] = src[p++];
      }
      w[w_p] = 0;
      if (strcpy(w, "include") == 0) {
        // handle include
      } else {
        // skip line
        while(!(src[p] == '\n' || src[p] == 0)) {
          p++;
        }
      }
    } else 

    // skip one line?
    if ( (src[p] == '/' && src[p+1] == '/') ) {
      while(!(src[p] == '\n' || src[p] == 0)) {
        p++;
      }
    } else
    // skip multiline comment
    if (src[p] == '/' && src[p+1] == '*') {
      while(!(src[p] == 0 || (src[p] != '/' && src[p-1] == '*') )) {
        p++;
      }
    }
    if (src[p] != 0) {
      p++;
    }
  }

}


