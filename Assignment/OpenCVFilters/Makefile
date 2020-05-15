CXX 		= g++ 
MYFLAGS 	= -O3 -std=c++17 -pthread
CXXFLAGS 	= -I /home/marcod/opencv4/include/opencv4
LDFLAGS  	= -L /home/marcod/opencv4/lib -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
OMPFLAG		= -fopenmp
FF_FLAG		= -I spm2020user/fastflow

TARGETS = filter_seq filter_omp filter_ff

all		: $(TARGETS) 

filter_seq:
	$(CXX) $(MYFLAGS) filter_seq.cpp -o filter_seq.o $(CXXFLAGS) $(LDFLAGS) 
filter_omp:
	$(CXX) $(MYFLAGS) filter_omp.cpp -o filter_omp.o $(CXXFLAGS) $(LDFLAGS) $(OMPFLAG) 
filter_ff:
	$(CXX) $(MYFLAGS) filter_ff.cpp -o filter_ff.o $(CXXFLAGS) $(LDFLAGS) $(FF_FLAG) 

