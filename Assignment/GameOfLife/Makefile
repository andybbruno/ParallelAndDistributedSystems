CXX			= g++ 
CXXFLAGS 	= -std=c++17
LDFLAGS 	= -pthread
OMP 		=
DEBUG		=


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),  Darwin)
	OMP	+= -Xpreprocessor -fopenmp -lomp
	DEBUG += -O3 -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize
else
	OMP	+= -fopenmp
	DEBUG += -O2 -ftree-vectorize -fopt-info-vec-all
endif



TARGETS 	= seq par_active par_passive par_omp
TARGETS_dbg = seq_dbg par_active_dbg par_passive_dbg par_omp_dbg

all		: $(TARGETS) 

debug	: $(TARGETS_dbg)

seq:
	$(CXX) $(CXXFLAGS) -O3 -o seq.o seq.cpp $< $(LDFLAGS) 
par_active:
	$(CXX) $(CXXFLAGS) -O3 -o par_active.o par_active.cpp $< $(LDFLAGS)
par_passive:
	$(CXX) $(CXXFLAGS) -O3 -o par_passive.o par_passive.cpp $< $(LDFLAGS)
par_omp:
	$(CXX) $(CXXFLAGS) -O3 -o par_omp.o par_omp.cpp $< $(LDFLAGS) $(OMP) 

seq_dbg:
	$(CXX) $(CXXFLAGS) -Wall -o seq.o seq.cpp $< $(LDFLAGS) $(DEBUG) 
par_active_dbg:
	$(CXX) $(CXXFLAGS) -Wall -o par_active.o par_active.cpp $< $(LDFLAGS) $(DEBUG) 
par_passive_dbg:
	$(CXX) $(CXXFLAGS) -Wall -o par_passive.o par_passive.cpp $< $(LDFLAGS) $(DEBUG) 
par_omp_dbg:
	$(CXX) $(CXXFLAGS) -Wall -o par_omp.o par_omp.cpp $< $(LDFLAGS) $(OMP) $(DEBUG) 
