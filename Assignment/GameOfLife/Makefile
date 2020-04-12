CXX			= g++ 
CXXFLAGS 	= -std=c++17 
OPTFLAGS	= -O3
LDFLAGS 	= -pthread


.PHONY: all clean cleanall
.SUFFIXES: .cpp 


%: %.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@.o $< $(LDFLAGS)

all		: $(TARGETS)

clean		: 
	rm -f $(TARGETS)

cleanall	: clean
	\rm -f *.o *~