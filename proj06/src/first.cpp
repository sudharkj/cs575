// 1. Program header
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <omp.h>
#include "CL/cl.h"
#include "CL/cl_platform.h"


#ifndef GLOBAL_SIZE
#define GLOBAL_SIZE 64*1024*1024
#endif

#define NUM_ELEMENTS GLOBAL_SIZE

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 64
#endif

#ifndef ADD
#define ADD 0
#endif

#ifndef REDUCE
#define REDUCE 1
#endif

#define NUM_WORK_GROUPS NUM_ELEMENTS/LOCAL_SIZE

const char *CL_FILE_NAME = {"first.cl"};
const double TOL = 0.0001f;

void Wait(cl_command_queue);

int LookAtTheBits(float);


int main(int argc, char *argv[]) {
    // see if we can even open the opencl kernel program
    // (no point going on if we can't):

    FILE *fp;
#ifdef WIN32
    errno_t err = fopen_s( &fp, CL_FILE_NAME, "r" );
    if( err != 0 )
#else
    fp = fopen(CL_FILE_NAME, "r");
    if (fp == NULL)
#endif
    {
        fprintf(stderr, "Cannot open OpenCL source file '%s'\n", CL_FILE_NAME);
        return 1;
    }

    cl_int status;        // returned status from opencl calls
    // test against CL_SUCCESS

    // get the platform id:
    cl_platform_id platform;
    status = clGetPlatformIDs(1, &platform, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clGetPlatformIDs failed (2)\n");

    // get the device id:
    cl_device_id device;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clGetDeviceIDs failed (2)\n");


    // 2. allocate the host memory buffers:

    double *hA = new double[NUM_ELEMENTS];
    double *hB = new double[NUM_ELEMENTS];
#if REDUCE
    double *hC = new double[NUM_WORK_GROUPS];
#elif ADD
    double *hC = new double[NUM_ELEMENTS];
    double *hD = new double[NUM_ELEMENTS];
#else
    double *hC = new double[NUM_ELEMENTS];
#endif

    // fill the host memory buffers:
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        hA[i] = hB[i] = (double) sqrt((double) i);
#if ADD && !REDUCE
        hC[i] = (double) i;
#endif
    }

    size_t inputSize = NUM_ELEMENTS * sizeof(double);
#if REDUCE
    size_t outputSize = NUM_WORK_GROUPS * sizeof(double);
#else
    size_t outputSize = inputSize;
#endif


    // 3. create an opencl context:
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateContext failed\n");


    // 4. create an opencl command queue:
    cl_command_queue cmdQueue = clCreateCommandQueue(context, device, 0, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateCommandQueue failed\n");


    // 5. allocate the device memory buffers:

    cl_mem dA = clCreateBuffer(context, CL_MEM_READ_ONLY, inputSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed (1)\n");

    cl_mem dB = clCreateBuffer(context, CL_MEM_READ_ONLY, inputSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed (2)\n");

#if ADD && !REDUCE
    cl_mem dC = clCreateBuffer(context, CL_MEM_READ_ONLY, inputSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed (3)\n");

    cl_mem dD = clCreateBuffer(context, CL_MEM_WRITE_ONLY, outputSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed (4)\n");
#else
    cl_mem dC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, outputSize, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateBuffer failed (3)\n");
#endif


    // 6. enqueue the 2 commands to write the data from the host buffers to the device buffers:

    status = clEnqueueWriteBuffer(cmdQueue, dA, CL_FALSE, 0, inputSize, hA, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueWriteBuffer failed (1)\n");

    status = clEnqueueWriteBuffer(cmdQueue, dB, CL_FALSE, 0, inputSize, hB, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueWriteBuffer failed (2)\n");

#if ADD && !REDUCE
    status = clEnqueueWriteBuffer(cmdQueue, dC, CL_FALSE, 0, inputSize, hC, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueWriteBuffer failed (3)\n");
#endif

    Wait(cmdQueue);


    // 7. read the kernel code from a file:
    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *clProgramText = new char[fileSize + 1];        // leave room for '\0'
    size_t n = fread(clProgramText, 1, fileSize, fp);
    clProgramText[fileSize] = '\0';
    fclose(fp);
    if (n != fileSize)
        fprintf(stderr, "Expected to read %d bytes read from '%s' -- actually read %d.\n", (int) fileSize, CL_FILE_NAME, (int) n);


    // create the text for the kernel program:
    char *strings[1];
    strings[0] = clProgramText;
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) strings, NULL, &status);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateProgramWithSource failed\n");
    delete[] clProgramText;


    // 8. compile and link the kernel code:
    char const *options = "";
    status = clBuildProgram(program, 1, &device, options, NULL, NULL);
    if (status != CL_SUCCESS) {
        size_t size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
        cl_char *log = new cl_char[size];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, size, log, NULL);
        fprintf(stderr, "clBuildProgram failed:\n%s\n", log);
        delete[] log;
    }

    // 9. create the kernel object:
#if REDUCE
    cl_kernel kernel = clCreateKernel(program, "ArrayMultReduce", &status);
#elif ADD
    cl_kernel kernel = clCreateKernel(program, "ArrayMultAdd", &status);
#else
    cl_kernel kernel = clCreateKernel(program, "ArrayMult", &status);
#endif
    if (status != CL_SUCCESS)
        fprintf(stderr, "clCreateKernel failed\n");


    // 10. setup the arguments to the kernel object:

    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dA);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (1)\n");

    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &dB);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (2)\n");

#if REDUCE
    // local “prods” array is dimensioned the size of each work-group
    status = clSetKernelArg(kernel, 2, LOCAL_SIZE * sizeof(double), NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (3)\n");

    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &dC);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (4)\n");
#elif ADD
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &dC);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (3)\n");

    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), &dD);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (4)\n");
#else
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &dC);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clSetKernelArg failed (3)\n");
#endif


    // 11. enqueue the kernel object for execution:

    size_t globalWorkSize[3] = {NUM_ELEMENTS, 1, 1};
    size_t localWorkSize[3] = {LOCAL_SIZE, 1, 1};

    Wait(cmdQueue);
    double time0 = omp_get_wtime();

    time0 = omp_get_wtime();

    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueNDRangeKernel failed: %d\n", status);

    Wait(cmdQueue);
    double time1 = omp_get_wtime();


    // 12. read the results buffer back from the device to the host:
#if ADD && !REDUCE
    status = clEnqueueReadBuffer(cmdQueue, dD, CL_TRUE, 0, outputSize, hD, 0, NULL, NULL);
#else
    status = clEnqueueReadBuffer(cmdQueue, dC, CL_TRUE, 0, outputSize, hC, 0, NULL, NULL);
#endif
    if (status != CL_SUCCESS)
        fprintf(stderr, "clEnqueueReadBuffer failed\n");

    // did it work?
#if REDUCE
    double obtained = 0.0;
    double expected = 0.0;
    for (int i = 0; i < NUM_WORK_GROUPS; i++) obtained += hC[i];
    for (int i = 0; i < NUM_ELEMENTS; i++) expected += (hA[i] * hB[i]);
    if (fabs(obtained - expected) > TOL) {
        fprintf( stderr, "[Reduction]: Sum wrongly produced %13.6f instead of %13.6f (%13.8f)\n",
                 obtained, expected, fabs(obtained-expected) );
        fprintf( stderr, "[Reduction]: Sum wrongly produced    0x%08x instead of    0x%08x\n",
                 LookAtTheBits(obtained), LookAtTheBits(expected) );
    }
#elif ADD
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        double expected = (hA[i] * hB[i]) + hC[i];
        double obtained = hD[i];
        if (fabs(obtained - expected) > TOL) {
            fprintf( stderr, "%4d: %13.6f * %13.6f + %13.6f wrongly produced %13.6f instead of %13.6f (%13.8f)\n",
            i, hA[i], hB[i], hC[i], obtained, expected, fabs(obtained-expected) );
            fprintf( stderr, "%4d:    0x%08x *    0x%08x +    0x%08x wrongly produced    0x%08x instead of    0x%08x\n",
            i, LookAtTheBits(hA[i]), LookAtTheBits(hB[i]), LookAtTheBits(hC[i]), LookAtTheBits(obtained), LookAtTheBits(expected) );
        }
    }
#else
    for (int i=0; i<NUM_ELEMENTS; i++) {
        double expected = hA[i] * hB[i];
        double obtained = hC[i];
        if (fabs(obtained - expected) > TOL) {
            fprintf( stderr, "%4d: %13.6f * %13.6f wrongly produced %13.6f instead of %13.6f (%13.8f)\n",
            i, hA[i], hB[i], obtained, expected, fabs(obtained-expected) );
            fprintf( stderr, "%4d:    0x%08x *    0x%08x wrongly produced    0x%08x instead of    0x%08x\n",
            i, LookAtTheBits(hA[i]), LookAtTheBits(hB[i]), LookAtTheBits(obtained), LookAtTheBits(expected) );
        }
    }
#endif

    fprintf(stderr, "%8d\t%4d\t%10d\t%10.3lf GigaMultsPerSecond\n",
            GLOBAL_SIZE, LOCAL_SIZE, NUM_WORK_GROUPS, (double) NUM_ELEMENTS / (time1 - time0) / 1000000000.);

    printf("%d,%d,%.3lf\n",
            GLOBAL_SIZE, LOCAL_SIZE, (double) NUM_ELEMENTS / (time1 - time0) / 1000000000.);

#ifdef WIN32
    Sleep( 2000 );
#endif


    // 13. clean everything up:
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(dA);
    clReleaseMemObject(dB);
    clReleaseMemObject(dC);
#if ADD && !REDUCE
    clReleaseMemObject(dD);
#endif

    delete[] hA;
    delete[] hB;
    delete[] hC;
#if ADD && !REDUCE
    delete[] hD;
#endif

    return 0;
}


int LookAtTheBits(float fp) {
    int *ip = (int *) &fp;
    return *ip;
}


// wait until all queued tasks have taken place:
void Wait(cl_command_queue queue) {
    cl_event wait;
    cl_int status;

    status = clEnqueueMarker(queue, &wait);
    if (status != CL_SUCCESS)
        fprintf(stderr, "Wait: clEnqueueMarker failed\n");

    status = clWaitForEvents(1, &wait);
    if (status != CL_SUCCESS)
        fprintf(stderr, "Wait: clWaitForEvents failed\n");
}
