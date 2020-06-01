CXX 		= g++ 
FLAGS 		= -O3 -std=c++17 -pthread
OMPFLAG		= -fopenmp
FF			= -g -I spm2020user/fastflow -finline-functions -DNDEBUG

TARGETS = seq farm_tasks farm_vectors master_workers openmp fastflow

all		: $(TARGETS)


seq:
	$(CXX) $(FLAGS) seq.cpp -o seq.o
farm_tasks:
	$(CXX) $(FLAGS) farm_tasks.cpp -o farm_tasks.o
farm_vectors:
	$(CXX) $(FLAGS) farm_vectors.cpp -o farm_vectors.o
master_workers:
	$(CXX) $(FLAGS) master_workers.cpp -o master_workers.o
openmp:
	$(CXX) $(FLAGS) openmp.cpp -o openmp.o $(OMPFLAG)
fastflow:
	$(CXX) $(FLAGS) fastflow.cpp -o fastflow.o $(FF)