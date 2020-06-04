CXX=g++
RM=rm
CUDA_PATH=/usr/local/cuda-10.2
CUDA_LIBS_PATH=$(CUDA_PATH)/lib64
OPENCL_LIBS=$(CUDA_LIBS_PATH)/libOpenCL.so.1.1
LDLIBS=-lm -fopenmp

MODE=0
NUMT=1
LOCAL_SIZE=64


all: proj printinfo

proj: proj.cpp not_parallel.h openmp_parallel.h cpu_simd.h gpu_opencl.h sin.cl
	$(CXX) -DMODE=$(MODE) -DNUMT=$(NUMT) -DLOCAL_SIZE=$(LOCAL_SIZE) proj.cpp $(OPENCL_LIBS) -o proj $(LDLIBS)

printinfo: print_info.cpp
	$(CXX) print_info.cpp $(OPENCL_LIBS) -o printinfo $(LDLIBS)

clean:
	$(RM) proj printinfo
