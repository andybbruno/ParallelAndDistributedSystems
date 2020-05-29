CXX 		= g++ 
MYFLAGS 	= -O3 -std=c++17 -pthread
OMPFLAG		= -fopenmp

TARGETS = par_farm par_omp

all		: $(TARGETS) 

par_farm:
	$(CXX) $(MYFLAGS) par_farm.cpp -o par_farm.o $(CXXFLAGS)
par_omp:
	$(CXX) $(MYFLAGS) par_omp.cpp -o par_omp.o $(CXXFLAGS) $(OMPFLAG)