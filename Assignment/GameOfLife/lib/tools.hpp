#include <vector>
#include <chrono>
#include <iostream>

#define ON 1
#define OFF 0

typedef std::vector<int16_t> Row;
typedef std::vector<std::vector<int16_t>> Table;
typedef std::pair<int16_t, Row> RowID;

struct Chunk
{
    int16_t id_curr;
    Row prev;
    Row curr;
    Row next;

    Chunk()
    {
        id_curr = 0;
        prev = Row(1, 0);
        curr = Row(1, 0);
        next = Row(1, 0);
    }

    Chunk(int16_t i)
    {
        id_curr = i;
        prev = Row(1, i);
        curr = Row(1, i);
        next = Row(1, i);
    }

    Chunk(int16_t i, Row const &a, Row const &b, Row const &c) : id_curr(i), prev(a), curr(b), next(c) {}

    inline bool operator==(const Chunk &rhs)
    {
        return (id_curr == rhs.id_curr) & (prev == rhs.prev) & (curr == rhs.curr) & (next == rhs.next);
    }
};


RowID EOC(-1, Row(1));
Chunk EOS(-1);


namespace tools
{
inline uint8_t neighbours(Table const &vec, uint64_t ix, uint64_t jx)
{
    auto alive = vec[ix][jx];
    uint8_t counter = 0;
    for (size_t i = ix - 1; i < ix + 2; i++)
    {
        for (size_t j = jx - 1; j < jx + 2; j++)
        {
            counter += vec[i][j];
        }
    }

    return (alive == 1) ? --counter : counter;
}

void create_glider(Table &vec, uint i, uint j)
{
    // Glider 1
    vec[1 + i][3 + j] = 1;
    vec[2 + i][1 + j] = 1;
    vec[2 + i][3 + j] = 1;
    vec[3 + i][2 + j] = 1;
    vec[3 + i][3 + j] = 1;
}

void randomize(Table &vec)
{
    uint n = vec.size();
    uint m = vec[0].size();

    for (size_t i = 1; i < n - 1; i++)
    {
        for (size_t j = 1; j < m - 1; j++)
        {
            vec[i][j] = rand() % 2;
        }
    }
}
void print(Table const &vec)
{
    int rows = vec.size();
    int cols = vec[0].size();

    std::cout << std::endl;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            bool b = vec[i][j];
            if (j == 0)
            {
                std::cout << (b == 1 ? "●" : " ") << "|";
            }
            else if (j == cols - 1)
            {
                std::cout << (b == 1 ? "●" : " ");
            }
            else
            {
                std::cout << (b == 1 ? "●" : " ") << "|";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
} // namespace tools