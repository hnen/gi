//
//  xxCommon.c
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "common.h"

int file_size(const char * filename, size_t * out_size);

XXerr file_read(const char * filename, XXstring * out_content, size_t * out_contentSize) {
    FILE *fp;
    size_t fsize;
    
    XX_E(!(fp = fopen(filename, "rb")));
    XX_E(file_size(filename, &fsize));
    
    char * buffer = (char*)xxalloc(fsize+1);
    size_t size = fread(buffer, 1, fsize, fp);
    fclose(fp);
    
    //buffer[fsize-1] = 'A';
    buffer[fsize] = 0;
    
    if (out_contentSize) {
        *out_contentSize = size;
    }
    *out_content = buffer;
    
    //printf("READ FILE: '%s'", *out_content);

    return 0;
}

XXerr file_size(const char * filename, size_t * out_size) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        *out_size = (size_t)st.st_size;
    }
   
    return 0;
}

void * __xxalloc(size_t amount) {
    size_t allocsize = amount+sizeof(void*)*2;
    void * mem = malloc(allocsize);
    memset(mem, 0xdf, allocsize);
    return mem + sizeof(void*);
}

void __xxensure(void * ptr, size_t alloc_size) {
    int m0 = *(unsigned int*)(ptr - 4) == 0xdfdfdfdf;
    int m1 = *(unsigned int*)(ptr + alloc_size) == 0xdfdfdfdf;
    if (!m0 || !m1) {
        printf(C_RED "XXENSURE FAIL" C_RESET "\n");
        __builtin_trap();
    }
}

void __xxfree(void * ptr) {
    free(ptr-sizeof(void*));
}
