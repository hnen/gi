//
//  xxCommon.c
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "xxCommon.h"

int file_size(XXstring filename, size_t * out_size);

XXerr file_read(XXstring filename, XXstring * out_content, size_t * out_contentSize) {
    /* Load the source code containing the kernel*/
    FILE *fp;
    size_t fsize;
    
    XX_E(!(fp = fopen(filename, "r")));
    XX_E(file_size(filename, &fsize));
    
    char * buffer = (char*)xxAlloc(fsize);
    size_t size = fread(buffer, 1, fsize, fp);
    fclose(fp);
    
    *out_contentSize = size;
    *out_content = buffer;
    
    return 0;
}

XXerr file_size(XXstring filename, size_t * out_size) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        *out_size = (size_t)st.st_size;
    }
   
    return 0;
}


