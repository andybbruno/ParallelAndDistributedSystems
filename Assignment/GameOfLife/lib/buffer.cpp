#include <iostream>
#include <atomic>

template <typename T>
class buffer
{
private:
  T item;
  std::atomic<bool> pbit;

public:
  buffer() : pbit(0) {}

  void send(T i)
  {
    while (pbit != 0);
    item = i;
    pbit = 1;
  }

  T receive()
  {
    while (pbit == 0);
    T v = item;
    pbit = 0;
    return v;
  }

  bool empty()
  {
    return pbit == 0;
  }
};
