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

#define XX_E(...) if((__VA_ARGS__)) { fprintf(stderr, "err @ %s(%s):%d", __func__, __FILE__, __LINE__); __builtin_trap(); }

#ifndef DEBUG
#define xxalloc(...) malloc(__VA_ARGS__)
#define xxfree(...) free(__VA_ARGS__)
#else
#define xxalloc(...) __xxalloc(__VA_ARGS__)
#define xxfree(...) __xxfree(__VA_ARGS__)
#endif

typedef char * XXstring;
typedef void(* XXcallback)();
typedef int XXerr;

XXerr file_read(const char * fileName, XXstring * out_content, size_t * out_contentSize);
void * __xxalloc(size_t amount);
void __xxfree(void * ptr);

static inline float clamp(float f) { return f < 0 ? 0 : (f > 1 ? 1 : f); }

#endif /* xxCommon_h */
