#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <cstddef>
#include <math.h>
#include <string>

//
// needed a blocking queue
// here is a sample queue
//

template <typename T>
class syque
{
private:
  std::mutex              d_mutex;
  std::condition_variable d_condition;
  std::deque<T>           d_queue;
public:

  syque(std::string s) { std::cout << "Created " << s << " queue " << std::endl;  }
  syque() {}
  
  void push(T const& value) {
    {
      std::unique_lock<std::mutex> lock(this->d_mutex);
      d_queue.push_front(value);
    }
    this->d_condition.notify_one();
  }
  
  T pop() {
    std::unique_lock<std::mutex> lock(this->d_mutex);
    this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
    T rc(std::move(this->d_queue.back()));
    this->d_queue.pop_back();
    return rc;
  }
};


//
// needed something to represent the EOS
// here we use null
//
#define EOS NULL

// loose some time
void active_delay(int msecs) {
  // read current time
  auto start = std::chrono::high_resolution_clock::now();
  auto end   = false;
  while(!end) {
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    if(msec>msecs)
      end = true;
  }
  return;
}

// double items in a vector, adding some delay
void double_vec_item(std::vector<double>& v, int msec) {
  for(int i=0; i<v.size(); i++) {
    active_delay(msec);
    v[i] = v[i]*2;
  }
  return;
}

// increases items in vector, adding some delay
void inc_vec_item(std::vector<double>& v,int msec) {
  for(int i=0; i<v.size(); i++) {
    active_delay(msec);
    v[i] = v[i]+1;
  }
  return;
}

// print items in a vector, just for check
void print_vec(std::vector<double> v) {
  for(auto it : v)
    std::cout << it << " ";
  std::cout << std::endl;
  return;
}
