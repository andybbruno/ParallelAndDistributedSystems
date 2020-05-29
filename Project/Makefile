CXX 		= g++ 
FLAGS 		= -O3 -std=c++17 -pthread
OMPFLAG		= -fopenmp

TARGETS = farm mawo omp

all		: $(TARGETS) 

farm_main:
	$(CXX) $(FLAGS) farm_main.cpp -o farm_main.o
mawo_main:
	$(CXX) $(FLAGS) mawo_main.cpp -o mawo_main.o
omp_main:
	$(CXX) $(FLAGS) omp_main.cpp -o omp_main.o $(OMPFLAG)