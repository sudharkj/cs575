CXX=g++
RM=rm
CUDA_PATH=/usr/local/cuda-10.2
CUDA_LIBS_PATH=$(CUDA_PATH)/lib64
OPENCL_LIBS=$(CUDA_LIBS_PATH)/libOpenCL.so.1.1
LDLIBS=-lm -fopenmp

ARRAY_SIZE=67108864
WORKER_SIZE=64
ADD=0
REDUCE=1


all: proj printinfo

proj: first.cpp first.cl
	$(CXX) -DADD=$(ADD) -DREDUCE=$(REDUCE) -DGLOBAL_SIZE=$(ARRAY_SIZE) -DLOCAL_SIZE=$(WORKER_SIZE) first.cpp $(OPENCL_LIBS) -o proj $(LDLIBS)

printinfo: printinfo.cpp
	$(CXX) printinfo.cpp $(OPENCL_LIBS) -o printinfo $(LDLIBS)

clean:
	$(RM) proj printinfo
