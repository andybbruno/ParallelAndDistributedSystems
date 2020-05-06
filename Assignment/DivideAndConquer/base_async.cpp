#include <thread>
#include <future>

int THREAD_THRESHOLD = 64;

template <typename Tin, typename Tout>
Tout dc(Tin input,
        bool (*basecase)(Tin &),
        Tout (*solve)(Tin &),
        std::vector<Tin> (*divide)(Tin &),
        Tout (*conquer)(std::vector<Tout> &))
{

    if (basecase(input))
    {
        return (solve(input));
    }
    else
    {
        auto subproblems = divide(input);
        std::vector<Tout> partials;

        if (THREAD_THRESHOLD > 0)
        {
            std::vector<std::future<Tout>> astids;

            for (auto prob : subproblems)
            {
                THREAD_THRESHOLD--;
                astids.push_back(std::async(dc<Tin,Tout>, prob, basecase, solve, divide, conquer));
            }

            for (int i = 0; i < astids.size(); i++)
            {
                auto res = astids[i].get();
                partials.push_back(res);
                // THREAD_THRESHOLD++;
            }
        }
        else
        {
            for (auto prob : subproblems)
            {
                auto res = dc<Tin, Tout>(prob, basecase, solve, divide, conquer);
                partials.push_back(res);
            }
        }

        auto result = conquer(partials);
        return (result);
    }
};