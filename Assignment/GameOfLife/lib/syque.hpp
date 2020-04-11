#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class syque
{
private:
    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::deque<T> d_queue;

public:
    void push(T const &value)
    {
        {
            std::unique_lock<std::mutex> lock(d_mutex);
            d_queue.push_front(value);
        }
        d_condition.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(d_mutex);
        d_condition.wait(lock, [=] { return !d_queue.empty(); });
        T rc(std::move(d_queue.back()));
        d_queue.pop_back();
        return rc;
    }

    T front()
    {
        return d_queue.front();
    }
};