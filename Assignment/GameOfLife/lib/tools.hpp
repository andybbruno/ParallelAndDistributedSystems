#pragma once
#include <vector>
#include <chrono>
#include <iostream>

namespace tools
{

struct Range
{
    uint16_t start;
    uint16_t end;
};

void delay(int msecs)
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

inline uint64_t neighbourhood(std::vector<std::vector<bool>> const &vec, uint64_t ix, uint64_t jx)
{
    uint64_t counter = 0;
    bool alive = vec[ix][jx];

    for (size_t i = ix - 1; i < ix + 2; i++)
    {
        for (size_t j = jx - 1; j < jx + 2; j++)
        {
            if (vec[i][j] == true)
                counter++;
        }
    }
    return alive ? --counter : counter;
}

void create_glider(std::vector<std::vector<bool>> &vec, uint i, uint j)
{
    // Glider 1
    vec[1 + i][3 + j] = true;
    vec[2 + i][1 + j] = true;
    vec[2 + i][3 + j] = true;
    vec[3 + i][2 + j] = true;
    vec[3 + i][3 + j] = true;
}

void randomize(std::vector<std::vector<bool>> &vec)
{
    uint n = vec.size();
    uint m = vec[0].size();

    for (size_t i = 1; i < n - 1; i++)
    {
        for (size_t j = 1; j < m - 1; j++)
        {
            vec[i][j] = (rand() % 2) == 0 ? false : true;
        }
    }
}
void print(std::vector<std::vector<bool>> const &vec)
{
    size_t rows = vec.size();
    size_t cols = vec[0].size();

    std::cout << std::endl;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            bool b = vec[i][j];
            if (j == 0)
            {
                std::cout << (b == true ? "●" : " ") << "|";
            }
            else if (j == cols - 1)
            {
                std::cout << (b == true ? "●" : " ");
            }
            else
            {
                std::cout << (b == true ? "●" : " ") << "|";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
} // namespace tools