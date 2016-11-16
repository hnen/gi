#if 0
//
//  xxCl.h
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#ifndef xxCl_h
#define xxCl_h

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

void cl_init(cl_context * out_context, cl_device_id * out_device_id);
void cl_run();

#endif /* xxCl_h */
#endif