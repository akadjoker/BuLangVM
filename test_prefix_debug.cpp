#include "interpreter.hpp"

int main()
{
    Interpreter vm;
    vm.registerAll();
    vm.run(R"(
        var x = 5;
        var y = ++x;
        print("x=" + x + " y=" + y);
    )", true);  // true para dump bytecode
    
    return 0;
}
