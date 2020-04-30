CXX=g++
RM=rm
USESIMD=0
USECORE=0
ARRAYSIZE=10000
LDLIBS=-lm -fopenmp

all: proj

proj: proj.cpp
	$(CXX) -DUSESIMD=$(USESIMD) -DUSECORE=$(USECORE) -DARRAYSIZE=$(ARRAYSIZE) proj.cpp -o proj $(LDLIBS)

clean:
	$(RM) proj
