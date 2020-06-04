//
// Created by sudha on 6/1/20.
//

#ifndef CS575_GPU_OPENCL_H
#define CS575_GPU_OPENCL_H

#include <fstream>
#include <iostream>
#include <omp.h>
#include <string.h>

using namespace std;

#include "CL/cl.h"
#include "CL/cl_platform.h"
#include "CL/cl_helper.h"

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 64
#endif

const string CL_FILE_NAME = "sin.cl";


double openCLAutoCorrelation(const float *hA, float *hSum, const int size) {
    // see if we can even open the opencl kernel program
    // (no point going on if we can't):
    ifstream clFile;
    // open file stream
    clFile.open(CL_FILE_NAME);
    if (!clFile) {
        cerr << "Cannot open OpenCL source file " << CL_FILE_NAME << endl;
        exit(1); // exit the program
    }

    // read the kernel code from a file:
    string clProgramText;
    // get file size
    clFile.seekg(0, ios::end);
    clProgramText.reserve(clFile.tellg());
    // read entire file
    clFile.seekg(0, ios::beg);
    clProgramText.assign(istreambuf_iterator<char>(clFile), istreambuf_iterator<char>());

    // close the file stream
    clFile.close();

    // declare status for opencl calls:
    cl_int status;

    // get the platform id:
    cl_platform_id platform;
    status = clGetPlatformIDs(1, &platform, NULL);
    printCLError(status, "clGetPlatformIDs failed (2)");

    // get the device id:
    cl_device_id device;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    printCLError(status, "clGetDeviceIDs failed (2)");

    // create an opencl context:
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
    printCLError(status, "clCreateContext failed");

    // create an opencl command queue:
    const cl_command_queue cmdQueue = clCreateCommandQueue(context, device, 0, &status);
    printCLError(status, "clCreateCommandQueue failed");

    // allocate the device memory buffers:
    const size_t inputSize = 2 * size * sizeof(float);
    const size_t outputSize = size * sizeof(float);
    const cl_mem dA = clCreateBuffer(context, CL_MEM_READ_ONLY, inputSize, NULL, &status);
    printCLError(status, "clCreateBuffer failed (1)");
    const cl_mem dSum = clCreateBuffer(context, CL_MEM_WRITE_ONLY, outputSize, NULL, &status);
    printCLError(status, "clCreateBuffer failed (2)");

    // enqueue the command to write the data from the host buffers to the device buffers:
    status = clEnqueueWriteBuffer(cmdQueue, dA, CL_FALSE, 0, inputSize, hA, 0, NULL, NULL);
    printCLError(status, "clEnqueueWriteBuffer failed (1)");
    // wait till the enqueue is complete
    clWait(cmdQueue);

    // create the text for the kernel program:
    const char *strings[1];
    strings[0] = clProgramText.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) strings, NULL, &status);
    printCLError(status, "clCreateProgramWithSource failed");

    // compile and link the kernel code:
    char const *options = "";
    status = clBuildProgram(program, 1, &device, options, NULL, NULL);
    if (status != CL_SUCCESS) {
        size_t size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
        cl_char *log = new cl_char[size];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, size, log, NULL);
        cerr << "clBuildProgram failed:" << endl << log << endl;
        delete[] log;
    }

    // create the kernel object:
    const cl_kernel kernel = clCreateKernel(program, "AutoCorrelate", &status);
    printCLError(status, "clCreateKernel failed");

    // setup the arguments to the kernel object:
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dA);
    printCLError(status, "clSetKernelArg failed (1)");
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &dSum);
    printCLError(status, "clSetKernelArg failed (2)");

    // enqueue the kernel object for execution:
    const size_t globalWorkSize[3] = {(size_t) size, 1, 1};
    const size_t localWorkSize[3] = {LOCAL_SIZE, 1, 1};

    // wait till all commands are complete
    clWait(cmdQueue);

    // do the computations on opencl
    const double time0 = omp_get_wtime();
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    printCLError(status, "clEnqueueNDRangeKernel failed");
    // wait till all commands are complete
    clWait(cmdQueue);
    const double time1 = omp_get_wtime();

    // read the results buffer back from the device to the host:
    status = clEnqueueReadBuffer(cmdQueue, dSum, CL_TRUE, 0, outputSize, hSum, 0, NULL, NULL);
    printCLError(status, "clEnqueueReadBuffer failed");

#ifdef WIN32
    Sleep( 2000 );
#endif

    // clean everything up:
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(dA);
    clReleaseMemObject(dSum);

    return time1 - time0;
}

#endif //CS575_GPU_OPENCL_H
