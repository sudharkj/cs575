## Autocorrelation

Solution to [Project #7b](http://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj07b.html) 
(Autocorrelation using CPU OpenMP, CPU SIMD, and GPU {OpenCL or CUDA}).

In this project a pattern in the given signal is detected using autocorrelation. 
The report documents this attempt to do autocorrelation and finding a pattern 
hidden in the signal using CPU OpenMP, CPU SIMD, and GPU OpenCL.

Additional details of this project is as follows:
* *signal.txt* contains sampled signal information, also available with course files
* *CL* contains files related to OpenCL
  * *cl.h* and *cl_platform.h* contains OpenCL package related code available with course files
  * *cl_helper.h* contains custom helper function to print error messages
* *not_parallel.h* contains computations without any optimizations
* *openmp_parallel.h* contains computations with OpenMP parallelism
* *simd_helper.h* contains dot product computation with CPU-SIMD
* *cpu-simd.h* contains computations with CPU-SIMD
* *openmp_simd.h* contains computations with CPU-SIMD and OpenMP parallelism
* *sin.cl* contains OpenCL kernel dot product computation code
* *gpu_opencl.h* contains computations with OpenCL parrallelism on GPU
* *print_info.cpp* contains platform and device information of OpenCL device, also available with course files

Other details can be found on [GitHub](https://github.com/sudharkj/cs575)
