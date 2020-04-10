#pragma once
#include "farm.hpp"
#include "tools.hpp"

#define END \
    Cell { 65535, 65535, 0 }

struct Cell
{
    uint16_t i;
    uint16_t j;
    bool alive;

    Cell(uint16_t a, uint16_t b, bool c) : i(a), j(b), alive(c){};

    inline bool
    operator==(const Cell &c)
    {
        return (i == c.i) & (j == c.j) & (alive == c.alive);
    }

    friend std::ostream &operator<<(std::ostream &out, const Cell &c)
    {
        auto status = c.alive ? "True" : "False";
        out << "[" << c.i << " , " << c.j << "] : " << status;
        return out;
    }
};

struct Farm
{
    class Emitter : IEmitter<Cell>
    {
    private:
        uint64_t rows;
        uint64_t cols;
        uint64_t max_elem;

        uint16_t i = 1;
        uint16_t j = 1;
        uint64_t current = 0;

        size_t &it;

        void increment()
        {
            ++current;
            i = (current / rows) + 1;
            j = (current % cols) + 1;
        }

    public:
        Emitter(uint16_t r, uint16_t c, size_t &iters) : rows(r - 2), cols(c - 2), it(iters)
        {
            max_elem = rows * cols;
        }

        Cell next()
        {
            auto res = Cell{i, j, false};
            increment();
            return res;
        }

        bool hasNext()
        {
            return (current < max_elem) & (it > 0);
        }
    };

    class Worker : IWorker<Cell>
    {
    private:
        const std::vector<std::vector<bool>> &matrix;
        std::vector<std::vector<bool>> &result;

    public:
        Worker(std::vector<std::vector<bool>> const &mat, std::vector<std::vector<bool>> &res) : matrix(mat), result(res) {}

        void compute(Cell c)
        {
            uint16_t row_ = c.i;
            uint16_t col_ = c.j;

            if (c == END)
                return;

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

            result[row_][col_] = alive;
        }
    };
};
