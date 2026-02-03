#include "interpreter.hpp"
#include <cstdio>

int main() {
    printf("Starting minimal benchmark...\n");
    
    Interpreter vm;
    printf("Interpreter created\n");
    
    vm.registerAll();
    printf("registerAll() done\n");
    
    bool ok = vm.run("var x = 5; x++;", false);
    printf("Test run: %s\n", ok ? "OK" : "FAIL");
    
    printf("Done\n");
    return 0;
}
