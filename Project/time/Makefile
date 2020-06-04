CXX 		= g++ 
FLAGS 		= -O3 -std=c++17 -pthread

TARGETS = farm_tasks_com farm_tasks_exec farm_chunks_com farm_chunks_exec

all		: $(TARGETS)


farm_tasks_com:
	$(CXX) $(FLAGS) farm_tasks_com.cpp -o farm_tasks_com.o
farm_tasks_exec:
	$(CXX) $(FLAGS) farm_tasks_exec.cpp -o farm_tasks_exec.o
farm_chunks_com:
	$(CXX) $(FLAGS) farm_chunks_com.cpp -o farm_chunks_com.o
farm_chunks_exec:
	$(CXX) $(FLAGS) farm_chunks_exec.cpp -o farm_chunks_exec.o
