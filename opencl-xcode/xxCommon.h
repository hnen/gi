//
//  xxCommon.h
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#ifndef xxCommon_h
#define xxCommon_h

#include <stdio.h>

#define XX_E(...) if((__VA_ARGS__)) { fprintf(stderr, "err @ %s(%s):%d", __func__, __FILE__, __LINE__); __builtin_trap(); }
#define xxAlloc(...) malloc(__VA_ARGS__)

typedef char * XXstring;
typedef void(* XXcallback)();
typedef int XXerr;

XXerr file_read(XXstring fileName, XXstring * out_content, size_t * out_contentSize);
XXstring xx_string(int len);

#endif /* xxCommon_h */
