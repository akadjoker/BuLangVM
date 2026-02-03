#include "interpreter.hpp"
#include <cstdio>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("========================================\n");
    printf("BuLang VM Test Suite\n");
    printf("========================================\n");

    {
        printf("\n[TEST] Simple print\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run("print(\"Hello World\");", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Prefix increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var x = 5;
            var y = ++x;
            print("x=" + x + " y=" + y);
            if (x == 6 && y == 6) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Postfix increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var x = 5;
            var y = x++;
            print("x=" + x + " y=" + y);
            if (x == 6 && y == 5) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Array\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var arr = [1, 2, 3];
            print("len=" + arr.length());
            if (arr.length() == 3) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Class with property increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            class Counter {
                var n = 0;
                def inc() { self.n++; }
                def get() { return self.n; }
            }
            var c = Counter();
            c.inc();
            c.inc();
            print("n=" + c.get());
            if (c.get() == 2) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    printf("\n========================================\n");
    printf("Tests complete\n");
    printf("========================================\n");

    return 0;
}
