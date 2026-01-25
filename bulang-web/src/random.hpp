#pragma once
#include <random>
#include <climits>


class RandomGenerator
{
private:
    std::mt19937 engine;

    RandomGenerator()
    {
        std::random_device rd;
        engine.seed(rd());
    }

public:
    static RandomGenerator &instance()
    {
        static RandomGenerator inst; // Thread-safe em C++11
        return inst;
    }

    // rand() - retorna [0, INT_MAX]
    int rand()
    {
        return std::uniform_int_distribution<int>(0, INT_MAX)(engine);
    }

    // rand(max) - retorna [0, max]
    int rand(int max)
    {
        return std::uniform_int_distribution<int>(0, max)(engine);
    }

    // rand(min, max) - retorna [min, max]
    int rand(int min, int max)
    {
        return std::uniform_int_distribution<int>(min, max)(engine);
    }

    // randFloat() - retorna [0.0, 1.0]
    double randFloat()
    {
        return std::uniform_real_distribution<double>(0.0, 1.0)(engine);
    }

    // randFloat(min, max)
    double randFloat(double min, double max)
    {
        return std::uniform_real_distribution<double>(min, max)(engine);
    }
};