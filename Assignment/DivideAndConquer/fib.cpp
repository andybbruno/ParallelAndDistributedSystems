#include <vector>
#include <iostream>
#include "utimer.cpp"
#include "base_async.cpp"

struct INTEGER
{
    uint x;
    uint size() { return x; }
    INTEGER(uint a) : x(a){};
};

bool basecase(INTEGER &v)
{
    return v.x < 2;
}

INTEGER solve(INTEGER &vec)
{
    return vec;
}

std::vector<INTEGER> divide(INTEGER &v)
{
    std::vector<INTEGER> res;
    res.push_back(v.x - 2);
    res.push_back(v.x - 1);
    return res;
}

INTEGER conquer(std::vector<INTEGER> &v)
{
    return v[0].x + v[1].x;
}

int main(int argc, char *argv[])
{
    utimer u("Fibonacci Parallel");

    uint a = atoi(argv[1]);
    INTEGER tmp(a);

    auto res = dc(tmp, basecase, solve, divide, conquer);
    std::cout << res.x << std::endl;
    return 0;
}