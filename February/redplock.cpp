#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <mutex>

std::mutex l;

#include "utimer.cpp"

#include <chrono>
void active_delay(int msecs)
{
  // read current time
  auto start = std::chrono::high_resolution_clock::now();
  auto end = false;
  while (!end)
  {
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    if (msec > msecs)
      end = true;
  }
  return;
}

// USED TO REPRESENT STRING CHUNKS
typedef struct
{
  int start;
  int end;
} RANGE;

int main(int argc, char *argv[])
{
  int n = atoi(argv[1]);    // vector lenght
  int seed = atoi(argv[2]); // seed
  int nw = atoi(argv[3]);   // number of workers
  srand(seed);

  std::vector<int> v(n);
  for (int i = 0; i < n; i++)
  {
    v[i] = rand() % 8;
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;

  {
    utimer u("reduce");

    std::vector<RANGE> ranges(nw); // vector to compute the ranges
    int delta{n / nw};
    std::vector<std::thread> tids;

    for (int i = 0; i < nw; i++)
    { // split the string into pieces
      ranges[i].start = i * delta;
      ranges[i].end = (i != (nw - 1) ? ((i + 1) * delta - 1) : (n - 1));
    }

    auto oplus = [](const int &x, const int &y) {
      auto r = x + y;
      active_delay(1);
      return (r);
    };
    int res = 0;
    auto compute_chunk = [&](RANGE r) {
      auto pres = v[r.start];
      for (int i = r.start + 1; i <= r.end; i++)
        pres = oplus(pres, v[i]);
      l.lock();
      res += pres;
      l.unlock();
      return;
    };

    for (int i = 0; i < nw; i++)
    { // assign chuncks to threads
      tids.push_back(std::thread(compute_chunk, ranges[i]));
    }

    for (std::thread &t : tids)
    { // await thread termination
      t.join();
    }
    std::cout << res << std::endl;
  }
  return (0);
}