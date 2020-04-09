#include <iostream>
#include <vector>
#include <grppi/map.h>
#include <dyn/dynamic_execution.h>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int n = atoi(argv[1]);
    grppi::dynamic_execution thr = grppi::parallel_execution_native{4};

    vector<int> in(n);
    vector<int> out(n);

    grppi::map(thr, begin(in), end(in), begin(in),
               [](int i){
                   return rand();
               });
    
    grppi::map(thr, begin(in), end(in), begin(out),
               [](int x) {
                   return x % 5;
               });

    for (int i : out)
    {
        cout << i << endl;
    }
    return 0;
}