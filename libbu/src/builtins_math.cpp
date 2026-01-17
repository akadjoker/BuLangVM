
#include "interpreter.hpp"
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



Value native_rand(Interpreter *vm, int argCount, Value *args)
{

    if (argCount == 0)
    {
        return vm->makeDouble(RandomGenerator::instance().randFloat());
    }
    else if (argCount == 1)
    {
        double value = args[0].asDouble();
        return vm->makeDouble(RandomGenerator::instance().randFloat(0, value));
    }
    else
    {
        double min = args[0].asDouble();
        double max = args[1].asDouble();
        return vm->makeDouble(RandomGenerator::instance().randFloat(min, max));
    }
    return vm->makeNil();
}

Value native_irand(Interpreter *vm, int argCount, Value *args)
{

    if (argCount == 0)
    {
        return vm->makeInt(RandomGenerator::instance().rand());
    }
    else if (argCount == 1)
    {
        int value = args[0].asInt();
        return vm->makeInt(RandomGenerator::instance().rand(0, value));
    }
    else
    {
        int min = args[0].asInt();
        int max = args[1].asInt();
        return vm->makeInt(RandomGenerator::instance().rand(min, max));
    }
    return vm->makeNil();
}

Value native_min(Interpreter *vm, int argCount, Value *args)
{

    bool isInt = args[0].isInt() && args[1].isInt();
    if (isInt)
    {
        return vm->makeInt(std::min(args[0].asInt(), args[1].asInt()));
    }

    return vm->makeDouble(std::min(args[0].asNumber(), args[1].asNumber()));    
}

Value native_max(Interpreter *vm, int argCount, Value *args)
{

    bool isInt = args[0].isInt() && args[1].isInt();
    if (isInt)
    {
        return vm->makeInt(std::max(args[0].asInt(), args[1].asInt()));
    }

    return vm->makeDouble(std::max(args[0].asNumber(), args[1].asNumber()));    
}

void Interpreter::registerMath()
{

    addModule("math")
        .addDouble("PI", 3.14159265358979)
        .addDouble("E", 2.71828182845905)
        .addFloat("SQRT2", 1.41421356f)
        .addInt("MIN_INT", -2147483648)
        .addInt("MAX_INT", 2147483647)
        .addFunction("min", native_min, 2)
        .addFunction("max", native_max, 2)
        .addFunction("rand", native_rand, -1)
        .addFunction("irand", native_irand, -1);

}

