#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        std::cout << "Usage is: " << argv[0] << " num_steps nw " << std::endl;
        return (-1);
    }
    long num_steps = atoi(argv[1]);
    int nw = atoi(argv[2]);

    double step;
    int i;
    double x, pi, sum = 0.0;

    auto start = omp_get_wtime();
#pragma omp parallel num_threads(nw)
    {
        step = 1.0 / (double)num_steps;
#pragma omp for private(i, x) reduction(+ : sum)
        for (i = 0; i < num_steps; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    }
    pi = step * sum;
    auto elapsed = omp_get_wtime() - start;
    std::cout << "Pi = " << pi << " (Computed in "
              << elapsed << " secs)" << std::endl;
    return (0);
}