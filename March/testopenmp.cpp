#include <iostream>
#include <omp.h>

int main()
{
#pragma omp parallel
    {
#pragma omp critical
        {
            auto id = omp_get_thread_num();
            std::cout << "ciao_" << id << std::endl;
        }
    }
    return 0;
}
