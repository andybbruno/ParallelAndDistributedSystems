CXX 		= g++ 
FLAGS 		= -O3 -std=c++17 -pthread

TARGETS = farm_tasks_com farm_tasks_exec farm_vectors_com farm_vectors_exec

all		: $(TARGETS)


farm_tasks_com:
	$(CXX) $(FLAGS) farm_tasks_com.cpp -o farm_tasks_com.o
farm_tasks_exec:
	$(CXX) $(FLAGS) farm_tasks_exec.cpp -o farm_tasks_exec.o
farm_vectors_com:
	$(CXX) $(FLAGS) farm_vectors_com.cpp -o farm_vectors_com.o
farm_vectors_exec:
	$(CXX) $(FLAGS) farm_vectors_exec.cpp -o farm_vectors_exec.o
