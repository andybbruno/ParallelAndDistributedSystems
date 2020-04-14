CXX			= g++ 
CXXFLAGS 	= -std=c++17 
OPTFLAGS	= -O3
LDFLAGS 	= -pthread -Wall
OMP 		=



UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),  Darwin)
	OMP	+= -Xpreprocessor -fopenmp -lomp
else
	OMP	+= -fopenmp
endif



TARGETS 	= seq par_active par_passive par_omp

all		: $(TARGETS) 

seq:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o seq.o seq.cpp $< $(LDFLAGS) 
par_active:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o par_active.o par_active.cpp $< $(LDFLAGS)
par_passive:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o par_passive.o par_passive.cpp $< $(LDFLAGS)
par_omp:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o par_omp.o par_omp.cpp $< $(LDFLAGS) $(OMP) 
