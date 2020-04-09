#pragma once
#include "farm.hpp"
#include "tools.hpp"

#define END = '0'

struct Cell
{
    uint16_t i;
    uint16_t j;
    bool alive;
};

struct Farm
{
    class Source : ISource<std::pair<uint64_t, uint64_t>>
    {
    private:
        uint64_t rows;
        uint64_t cols;
        uint64_t max_elem;

        uint64_t i = 1;
        uint64_t j = 1;
        uint64_t current = 0;

        void increment()
        {
            ++current;
            i = (current / rows) + 1;
            j = (current % cols) + 1;
        }

    public:
        Source(uint64_t r, uint64_t c) : rows(r - 2), cols(c - 2)
        {
            max_elem = rows * cols;
        }

        std::pair<uint64_t, uint64_t> next()
        {
            auto res = std::make_pair(i, j);
            increment();
            return res;
        }

        bool hasNext()
        {
            return current < max_elem;
        }
    };

    class Worker : IWorker<std::pair<uint16_t, uint16_t>, Cell>
    {
    private:
        const std::vector<std::vector<bool>> &matrix;

    public:
        Worker(std::vector<std::vector<bool>> const &mat) : matrix(mat) {}

        Cell compute(std::pair<uint16_t, uint16_t> c)
        {
            uint16_t row_ = c.first;
            uint16_t col_ = c.second;

            int neig = tools::neighbourhood(matrix, row_, col_);
            bool alive = matrix[row_][col_];

            if (alive)
            {
                if ((neig < 2) | (neig > 3))
                {
                    alive = false;
                }
                else if ((neig == 2) | (neig == 3))
                {
                    alive = true;
                }
            }
            else
            {
                if (neig == 3)
                {
                    alive = true;
                }
            }

            return Cell{row_, col_, alive};
        }
    };

    class Drain : IDrain<Cell>
    {
    private:
        std::vector<std::vector<bool>> &matrix;

    public:
        Drain(std::vector<std::vector<bool>> &mat) : matrix(mat) {}
        void process(Cell c)
        {
            matrix[c.i][c.j] = c.alive;
        }
    };
};
