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

    {
        printf("\n[TEST] Prefix decrement\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var a = 10;
            var b = --a;
            print("a=" + a + " b=" + b);
            if (a == 9 && b == 9) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Postfix decrement\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var a = 10;
            var b = a--;
            print("a=" + a + " b=" + b);
            if (a == 9 && b == 10) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Multiple increments in expression\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var a = 1;
            var b = 2;
            var result = ++a + ++b;
            print("a=" + a + " b=" + b + " result=" + result);
            if (a == 2 && b == 3 && result == 5) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Increment in loop\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var sum = 0;
            var i = 0;
            while (i < 5) {
                sum = sum + ++i;
            }
            print("sum=" + sum + " i=" + i);
            if (sum == 15 && i == 5) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Local variable increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            def test() {
                var local = 5;
                var result = ++local;
                return local == 6 && result == 6;
            }
            var ok = test();
            if (ok) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Mixed prefix and postfix\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var a = 5;
            var b = a++ + ++a;
            print("a=" + a + " b=" + b);
            if (a == 7 && b == 12) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Increment with negative numbers\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            var a = -5;
            var b = ++a;
            var c = a++;
            print("a=" + a + " b=" + b + " c=" + c);
            if (a == -3 && b == -4 && c == -4) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Function with local increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            def testFunc() {
                var counter = 0;
                var r1 = ++counter;
                var r2 = counter++;
                var r3 = ++counter;
                return [r1, r2, r3, counter];
            }
            var result = testFunc();
            print("r1=" + result[0] + " r2=" + result[1] + " r3=" + result[2] + " counter=" + result[3]);
            if (result[0] == 1 && result[1] == 1 && result[2] == 3 && result[3] == 3) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Class method with increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            class Counter {
                var value = 0;
                def increment() {
                    var local = 5;
                    local++;
                    self.value++;
                    return local;
                }
                def prefixInc() {
                    var local = 10;
                    return ++local;
                }
            }
            var c = Counter();
            var r1 = c.increment();
            var r2 = c.increment();
            var r3 = c.prefixInc();
            print("r1=" + r1 + " r2=" + r2 + " r3=" + r3 + " value=" + c.value);
            if (r1 == 6 && r2 == 6 && r3 == 11 && c.value == 2) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Nested function increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            def outer() {
                var x = 5;
                def inner() {
                    x++;
                    return ++x;
                }
                var result = inner();
                return [result, x];
            }
            var arr = outer();
            print("result=" + arr[0] + " x=" + arr[1]);
            if (arr[0] == 7 && arr[1] == 7) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Function parameters and increment\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            def modify(n) {
                var r1 = n++;
                var r2 = ++n;
                return [r1, r2, n];
            }
            var result = modify(10);
            print("r1=" + result[0] + " r2=" + result[1] + " n=" + result[2]);
            if (result[0] == 10 && result[1] == 12 && result[2] == 12) {
                print("OK");
            } else {
                print("FAIL");
            }
        )", false);
        printf("  %s\n", ok ? "PASS" : "FAIL");
    }

    {
        printf("\n[TEST] Class with multiple methods incrementing\n");
        Interpreter vm;
        vm.registerAll();
        bool ok = vm.run(R"(
            class Calculator {
                var total = 0;
                def addPrefix(val) {
                    return self.total = self.total + (++val);
                }
                def addPostfix(val) {
                    return self.total = self.total + (val++);
                }
            }
            var calc = Calculator();
            var r1 = calc.addPrefix(5);
            var r2 = calc.addPostfix(10);
            print("r1=" + r1 + " r2=" + r2 + " total=" + calc.total);
            if (r1 == 6 && r2 == 16 && calc.total == 16) {
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
