#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <mutex>

std::mutex l;

#include "utimer.cpp"

#include <chrono>

//
// active wait : this has been used only to emulate some actual
// work (business logic code) consuming computing resources on
// our processor / core. Not to be used in normal (true) appl code.
//
void active_delay(int msecs)
{
  // read current time
  auto start = std::chrono::high_resolution_clock::now();
  auto end = false;
  while (!end)
  {
    auto elapsed =
        std::chrono::high_resolution_clock::now() - start;
    auto msec =
        std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
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

  // always check for correct number of arguments and, in case
  // print expected argument list and terminate with some error code.
  // As an alternative, lack of arguments may be dealt with by
  // assigning some default argument values ...

  if (argc != 4)
  {
    std::cout << "Usage:\n"
              << argv[0] << " n seed nw " << std::endl;
    return (-1);
  }

  // argument parsing
  int n = atoi(argv[1]);    // vector lenght
  int seed = atoi(argv[2]); // seed
  int nw = atoi(argv[3]);   // number of workers

  // always use the same seed when repeating the same experiment
  srand(seed);

  // random vector initialization
  std::vector<int> v(n);
  for (int i = 0; i < n; i++)
  {
    v[i] = rand() % 8;
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;

  {
    utimer u("reduce");

    std::vector<std::pair<int, int>> ranges(nw);
    std::vector<std::thread> tids;

    // // PROF SOLUTION
    // int delta{n / nw};
    // for (int i = 0; i < nw; i++)
    // {
    //   ranges[i] = std::make_pair(i * delta,
    //                              (i != (nw - 1) ? (i + 1) * delta - 1 : n - 1));
    //   std::cout << "ranges[" << i << "] " << ranges[i].first << " , "
    //             << ranges[i].second << std::endl;
    // }


    // MY SOLUTION
    int delta{n / nw};
    int mod{n % nw};

    for (int i = 0; i < nw; i++)
    {
      int a = (i != 0 ? ranges[i - 1].second + 1 : 0);
      int b = (i - mod < 0 ? a + delta : a + delta - 1);
      ranges[i] = std::make_pair(a,b);
      std::cout << "ranges[" << i << "] " << ranges[i].first << " , "
                << ranges[i].second << std::endl;
    }

    // this is the "accumulate" function. The const int& types are required
    // by the std::accumulate we use in the sequential version; we kept them
    // just to use the very same business logic code. No implications on the
    // time spent to compute the oplus function calls, indeed

    auto oplus = [](const int &x, const int &y) {
      auto r = x + y;
      active_delay(1);
      return (r);
    };

    int res = 0;

    // this is the the thread body function. Simply compute locally the
    // partial sum, then lock the global sum value and update it in
    // mutual exclusion with the rest of the threads

    auto compute_chunk =
        [&](std::pair<int, int> r) {
          auto pres = v[r.first];
          for (int i = r.first + 1; i <= r.second; i++)
            pres = oplus(pres, v[i]);
          l.lock();
          res += pres;
          l.unlock();
          return;
        };

    // fork/join model parallel computation:
    // fork nw threads, each executing thread body function on
    // the assigned partition (chunk) of input data

    for (int i = 0; i < nw; i++)
    {
      tids.push_back(std::thread(compute_chunk, ranges[i]));
    }

    // once the threads have been created, leave them work and
    // finally await all of them terminate.

    for (std::thread &t : tids)
    {
      t.join();
    }

    // done: here is the final accumulation result
    std::cout << res << std::endl;
  }
  return (0);
}