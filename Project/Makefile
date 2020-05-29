CXX 		= g++ 
MYFLAGS 	= -O3 -std=c++17 -pthread
OMPFLAG		= -fopenmp

TARGETS = farm mawo omp

all		: $(TARGETS) 

farm:
	$(CXX) $(MYFLAGS) farm_main.cpp -o farm_main.o $(CXXFLAGS)
mawo:
	$(CXX) $(MYFLAGS) mawo_main.cpp -o mawo_main.o $(CXXFLAGS)
omp:
	$(CXX) $(MYFLAGS) omp_main.cpp -o omp_main.o $(CXXFLAGS) $(OMPFLAG)