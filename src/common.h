//
//  xxCommon.h
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"

#define XX_E(...) if((__VA_ARGS__)) { fprintf(stderr, "err @ %s(%s):%d", __func__, __FILE__, __LINE__); __builtin_trap(); }

#define XX_TODO(...) { char tmp[1024]; sprintf(tmp, __VA_ARGS__); fprintf(stderr, "unimplemented code @ %s(%s):%d \"%s\"", __func__, __FILE__, __LINE__, tmp); __builtin_trap(); }

#ifndef DEBUG
#define xxalloc(...) malloc(__VA_ARGS__)
#define xxassert_alloc(...) 
#define xxfree(...) free(__VA_ARGS__)
#else

void __xxensure(void * ptr, size_t alloc_size);

#define xxalloc(...) __xxalloc(__VA_ARGS__)
#define xxassert_alloc(...) __xxensure(__VA_ARGS__)
#define xxfree(...) __xxfree(__VA_ARGS__)
#endif

#define xxfloat float

#define randf() ((xxfloat)((double)rand() / (double)RAND_MAX))

typedef char * XXstring;
typedef void(* XXcallback)();
typedef int XXerr;

XXerr file_read(const char * fileName, XXstring * out_content, size_t * out_contentSize);
void * __xxalloc(size_t amount);
void __xxfree(void * ptr);

static inline xxfloat clamp(xxfloat f) { return f < 0 ? 0 : (f > 1 ? 1 : f); }

#endif /* xxCommon_h */
