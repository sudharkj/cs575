CXX=g++
RM=rm
NUMT=1
NUMNODES=4
LDLIBS=-lm -fopenmp

all: proj

proj: proj.cpp
	$(CXX) -DNUMT=$(NUMT) -DNUMNODES=$(NUMNODES) proj.cpp -o proj $(LDLIBS)

clean:
	$(RM) proj
