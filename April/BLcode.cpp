// this is the business logic code
#include <iostream>
#include "BLcode.hpp"
#include <chrono>
#include <thread>

// tasks to be computed: stream of integers, provided as iterator
class MySource : public Source<int> {
private:
  int start;
  int stop;
  int msec;
public:
  MySource(int from, int to, int ms) : start(from), stop(to), msec(ms) {}
  MySource() : start(0), stop(1000000), msec(100) {}

  int next() {
    std::this_thread::sleep_for (std::chrono::milliseconds(msec));
    return(start++);
  }

  bool hasNext() {
    if(start==stop)
      return(false);
    else
      return(true);
  }
};

// business logic to compute a task
class MyWorker : public Worker<int,int> {
private:
  int msec;
public:
  MyWorker(int ms): msec(ms) {}
  MyWorker() : msec(500) {}
  
  int compute(int x) {
    std::this_thread::sleep_for (std::chrono::milliseconds(msec));
    return(++x);
  }
};

// processing the results: accumulate the stream contents
class MyDrain : public Drain<int> {
private:
  long sum = 0;
  int msec;
  
public:

  MyDrain(int ms) : msec(ms) {}
  MyDrain() : msec(100) {}
  
    void process(int x) {
      std::this_thread::sleep_for (std::chrono::milliseconds(msec));
      sum += x;
      std::cout << " " << sum;
      return;
    }

};
