#include <vector>
#include "lib/utimer.cpp"
int main()
{
    utimer u("");

    
    std::vector<int> v(1000000000);
    for (size_t i = 1; i < v.size(); i++)
    {
        v[i] = i * i;
    }
    return 0;
}