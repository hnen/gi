
#include <string.h>
#include "common.h"

void get_path_prefix(const char * path, char ** out_path_prefix) {
    int p = strlen(path)-1;
    while(p >= 0 && path[p] != '/') p--;
    *out_path_prefix = xxalloc(p+1+1);
    if (p >= 0) {
        memcpy(*out_path_prefix, path, p+1);
    }
    (*out_path_prefix)[p+1] = 0;
    xxassert_alloc(*out_path_prefix, p+1+1);
}

// Concantenates str0[a..b] + str1 + str0[c..d]
void concat(const char * str0, int a, int b, const char * str1, int c, int d, char ** out_string) {

    /*
    char * tmpstr = xxalloc(strlen(str0)+1);
    strcpy(tmpstr, str0);
    char _x = tmpstr[b];
    tmpstr[b] = 0;
    printf(C_CYAN "CONCAT:\n---\n%s\n---\n", &tmpstr[a]);
    tmpstr[b] = _x;
    printf(C_CYAN "%s\n---\n", str1);
    _x = tmpstr[d];
    tmpstr[d] = 0;
    printf(C_CYAN "%s\n---\n", &tmpstr[c]);
    tmpstr[d] = _x;
    xxfree(tmpstr);
    */

    int str1_len = strlen(str1);
    size_t allocsize = (b-a) + (d-c) + str1_len + 1;
    char * ret = xxalloc(allocsize);
    memcpy(&ret[0],              &str0[a], b-a);
    memcpy(&ret[b-a],             str1,    str1_len);
    memcpy(&ret[b-a + str1_len], &str0[c], d-c);
    ret[b-a + str1_len + d-c] = 0;
    *out_string = ret;
    printf(C_GREEN "CONCAT RESULT: %s" C_RESET, ret);
    xxassert_alloc(ret, allocsize);
}

void _glsl_preproc_process(const char * src, char * path_prefix, char ** out_src) {
    int p = 0;

    int p_offset = 0;

    *out_src = xxalloc(strlen(src)+1);
    strcpy(*out_src, src);
    xxassert_alloc(*out_src, strlen(src)+1);

    while(src[p] != 0) {
        int p0 = p;
        if (src[p] == '#') {
            if (memcmp(&src[p+1], "include", strlen("include")) == 0) {
                p += 8;
                while(src[p] != '\"') p++;
                p++;
                char filename[1024];
                int i = 0;
                while(src[p] != '\"') {
                    filename[i++] = src[p++];
                }
                filename[i] = 0;
                char path[1024];
                sprintf(path, "%s%s", path_prefix, filename);

                char * included_src;
                char * included_src_preprocessed;
                char * included_file_path_prefix;

                file_read(path, &included_src, 0);
                get_path_prefix(path, &included_file_path_prefix);
                _glsl_preproc_process(included_src, included_file_path_prefix, &included_src_preprocessed);

                int p1 = p;
                while(src[p1] != '\n') p1++;

                char * concatenated_src;
                concat(*out_src, 0, p0 + p_offset, included_src_preprocessed, p1+1+p_offset, strlen(*out_src), &concatenated_src);

                p_offset += strlen(included_src_preprocessed) - (p1+1-p0);

                xxfree(*out_src);
                xxfree(included_src_preprocessed);
                xxfree(included_src);
                xxfree(included_file_path_prefix);


                *out_src = concatenated_src;

                while(!(src[p] == '\n' || src[p] == 0)) {
                    p++;
                }
            }
        } else 
        // skip one line comment
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


void glsl_preproc_process(const char * filename, char ** out_src) {
    char * content;
    char * pathprefix;
    file_read(filename, &content, 0);
    get_path_prefix(filename, &pathprefix);
    _glsl_preproc_process(content, pathprefix, out_src);
    xxfree(pathprefix);
    xxfree(content);
}

