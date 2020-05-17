CUDA_PATH=/usr/local/cuda-10.2
CUDA_BIN_PATH=$(CUDA_PATH)/bin
CUDA_NVCC=$(CUDA_BIN_PATH)/nvcc

NUMTRIALS=32768
BLOCKSIZE=16


all: proj

proj: montecarloTemplate.cu
	$(CUDA_NVCC) -DNUMTRIALS=$(NUMTRIALS) -DBLOCKSIZE=$(BLOCKSIZE) montecarloTemplate.cu -o proj

clean:
	$(RM) proj
