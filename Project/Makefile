CXX 		= g++ 
FLAGS 		= -O3 -std=c++17 -pthread
OMPFLAG		= -fopenmp
FF			= -I spm2020user/fastflow -finline-functions

TARGETS = seq farm_tasks farm_chunks master_workers openmp ff_master_workers ff_farm_tasks

all		: $(TARGETS)


seq:
	$(CXX) $(FLAGS) seq.cpp -o seq.o
farm_tasks:
	$(CXX) $(FLAGS) farm_tasks.cpp -o farm_tasks.o
farm_chunks:
	$(CXX) $(FLAGS) farm_chunks.cpp -o farm_chunks.o
master_workers:
	$(CXX) $(FLAGS) master_workers.cpp -o master_workers.o
openmp:
	$(CXX) $(FLAGS) openmp.cpp -o openmp.o $(OMPFLAG)
ff_master_workers:
	$(CXX) $(FLAGS) ff_master_workers.cpp -o ff_master_workers.o $(FF)
ff_farm_tasks:
	$(CXX) $(FLAGS) ff_farm_tasks.cpp -o ff_farm_tasks.o $(FF)