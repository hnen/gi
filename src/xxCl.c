//
//  xxCl.c
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#include "xxCommon.h"
#include "xxCl.h"

void cl_run() {
    cl_context context = NULL;
    cl_device_id device_id = NULL;
    
    cl_command_queue command_queue = NULL;
    cl_mem memobj = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_int ret;
    
    char string[MEM_SIZE];
    
    char *source_str;
    size_t source_size;
    file_read("./kernels/hello.cl", &source_str, &source_size);
    
    cl_init(&context, &device_id);
    
    /* Build Kernel Program */
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, &source_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "hello", &ret);
    
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);
    
    /* Execute OpenCL Kernel */
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
    ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
    
    /* Copy results from the memory buffer */
    ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
                              MEM_SIZE * sizeof(char),string, 0, NULL, NULL);
    
    /* Display Result */
    puts(string);
    
    /* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memobj);
    ret = clReleaseContext(context);
    
    xxfree(source_str);
    
}

void cl_init(cl_context * out_context, cl_device_id * out_device_id) {
    cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_device_id device_id = NULL;
    cl_int ret;
    
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    *out_context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    *out_device_id = device_id;
}
