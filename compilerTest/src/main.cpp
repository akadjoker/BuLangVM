 

#include "compiler.hpp"
#include "interpreter.hpp"
#include <iostream>
#include <string>

int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

void test(const char* category, const char* testName, const char* code, bool shouldCompile = true)
{
    totalTests++;
    
    Interpreter vm;
 
    
    Compiler compiler(&vm);
    ProcessDef* result = compiler.compile(code);
    
    auto stats = compiler.getStats();
    
    bool passed = (result != nullptr) == shouldCompile;
    
    if (passed)
    {
        passedTests++;
        std::cout << "✓ [" << category << "] " << testName;
        if (!shouldCompile)
            std::cout << " (erro esperado: " << stats.totalErrors << ")";
        std::cout << std::endl;
    }
    else
    {
        failedTests++;
        std::cout << "✗ [" << category << "] " << testName;
        if (shouldCompile)
            std::cout << " (devia compilar, mas falhou com " << stats.totalErrors << " erros)";
        else
            std::cout << " (não devia compilar, mas compilou)";
        std::cout << std::endl;
    }
}

int main()
{
    std::cout << "╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  SUITE COMPLETA DE TESTES - BuLang    ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // ========================================
    // TIPOS DE DADOS
    // ========================================
    
    test("TIPOS", "Integer positivo", "var x = 42;");
    test("TIPOS", "Integer negativo", "var x = -42;");
    test("TIPOS", "Integer hex", "var x = 0xFF;");
    test("TIPOS", "Float", "var x = 3.14;");
    test("TIPOS", "Float negativo", "var x = -3.14;");
    test("TIPOS", "String", R"(var s = "hello";)");
    test("TIPOS", "String vazia", R"(var s = "";)");
    test("TIPOS", "String com escapes", R"(var s = "hello\nworld\t!";)");
    test("TIPOS", "Boolean true", "var b = true;");
    test("TIPOS", "Boolean false", "var b = false;");
    test("TIPOS", "Nil", "var n = nil;");
    
    // ========================================
    // OPERADORES ARITMÉTICOS
    // ========================================
    
    test("ARITMÉTICA", "Adição", "var x = 1 + 2;");
    test("ARITMÉTICA", "Subtração", "var x = 5 - 3;");
    test("ARITMÉTICA", "Multiplicação", "var x = 4 * 3;");
    test("ARITMÉTICA", "Divisão", "var x = 10 / 2;");
    test("ARITMÉTICA", "Módulo", "var x = 10 % 3;");
    test("ARITMÉTICA", "Negação", "var x = -42;");
    test("ARITMÉTICA", "Expressão complexa", "var x = (1 + 2) * 3 - 4 / 2;");
    
    // ========================================
    // OPERADORES LÓGICOS
    // ========================================
    
    test("LÓGICA", "AND", "var x = true && false;");
    test("LÓGICA", "OR", "var x = true || false;");
    test("LÓGICA", "NOT", "var x = !true;");
    test("LÓGICA", "Comparação ==", "var x = 5 == 5;");
    test("LÓGICA", "Comparação !=", "var x = 5 != 3;");
    test("LÓGICA", "Comparação <", "var x = 3 < 5;");
    test("LÓGICA", "Comparação <=", "var x = 5 <= 5;");
    test("LÓGICA", "Comparação >", "var x = 5 > 3;");
    test("LÓGICA", "Comparação >=", "var x = 5 >= 5;");
    
    // ========================================
    // OPERADORES BITWISE
    // ========================================
    
    test("BITWISE", "AND", "var x = 0xFF & 0x0F;");
    test("BITWISE", "OR", "var x = 0xF0 | 0x0F;");
    test("BITWISE", "XOR", "var x = 0xFF ^ 0x0F;");
    test("BITWISE", "NOT", "var x = ~0xFF;");
    test("BITWISE", "Shift left", "var x = 1 << 4;");
    test("BITWISE", "Shift right", "var x = 16 >> 2;");
    
    // ========================================
    // ARRAYS
    // ========================================
    
    test("ARRAYS", "Array vazio", "var arr = [];");
    test("ARRAYS", "Array com elementos", "var arr = [1, 2, 3];");
    test("ARRAYS", "Array aninhado", "var arr = [[1, 2], [3, 4]];");
    test("ARRAYS", "Array subscript get", "var arr = [1, 2]; var x = arr[0];");
    test("ARRAYS", "Array subscript set", "var arr = [1, 2]; arr[0] = 10;");
    test("ARRAYS", "Array length", "var arr = [1, 2, 3]; var l = len(arr);");
    
    // ========================================
    // MAPS
    // ========================================
    
    test("MAPS", "Map vazio", "var m = {};");
    test("MAPS", "Map com elementos", R"(var m = {a: 1, b: 2};)");
    test("MAPS", "Map com string keys", R"(var m = {"key": "value"};)");
    test("MAPS", "Map dot access", R"(var m = {x: 10}; var v = m.x;)");
    test("MAPS", "Map subscript", R"(var m = {x: 10}; var v = m["x"];)");
    
    // ========================================
    // BUFFERS
    // ========================================
    
    test("BUFFERS", "Buffer creation", "var buf = @(10, 1);");
    test("BUFFERS", "Buffer subscript", "var buf = @(10, 1); var b = buf[0];");
    test("BUFFERS", "Buffer subscript set", "var buf = @(10, 1); buf[0] = 42;");
    
    // ========================================
    // STRINGS
    // ========================================
    
    test("STRINGS", "String escape \\n", R"(var s = "hello\nworld";)");
    test("STRINGS", "String escape \\t", R"(var s = "hello\tworld";)");
    test("STRINGS", "String escape \\\"", R"(var s = "say \"hi\"";)");
    test("STRINGS", "String escape hex", R"(var s = "\x41";)");
    test("STRINGS", "String escape unicode", R"(var s = "\u0041";)");
    test("STRINGS", "String concatenação", R"(var s = "hello" + " world";)");
    
    // ========================================
    // VARIÁVEIS
    // ========================================
    
    test("VARIÁVEIS", "Declaração simples", "var x = 10;");
    test("VARIÁVEIS", "Declaração múltipla", "var x = 1, y = 2, z = 3;");
    test("VARIÁVEIS", "Declaração sem inicialização", "var x;");
    test("VARIÁVEIS", "Atribuição", "var x = 10; x = 20;");
    test("VARIÁVEIS", "Increment ++", "var x = 5; ++x;");
    test("VARIÁVEIS", "Decrement --", "var x = 5; --x;");
    test("VARIÁVEIS", "Compound +=", "var x = 5; x += 3;");
    test("VARIÁVEIS", "Compound -=", "var x = 5; x -= 3;");
    test("VARIÁVEIS", "Compound *=", "var x = 5; x *= 3;");
    test("VARIÁVEIS", "Compound /=", "var x = 15; x /= 3;");
    
    // ========================================
    // FUNÇÕES
    // ========================================
    
    test("FUNÇÕES", "Função sem parâmetros", "def foo() { return 42; }");
    test("FUNÇÕES", "Função com parâmetros", "def add(a, b) { return a + b; }");
    test("FUNÇÕES", "Função recursiva", "def fib(n) { if (n <= 1) return n; return fib(n-1) + fib(n-2); }");
    test("FUNÇÕES", "Chamada de função", "def foo() { return 42; } var x = foo();");
    test("FUNÇÕES", "Função aninhada", "def outer() { def inner() { return 42; } return inner(); }");
    
    // ========================================
    // CONTROLE DE FLUXO
    // ========================================
    
    test("IF", "If simples", "if (true) { var x = 1; }");
    test("IF", "If-else", "if (true) { var x = 1; } else { var x = 2; }");
    test("IF", "If-elif-else", "if (false) { } elif (true) { } else { }");
    
    test("LOOPS", "While", "var i = 0; while (i < 10) { i = i + 1; }");
    test("LOOPS", "Do-while", "var i = 0; do { i = i + 1; } while (i < 10);");
    test("LOOPS", "For", "for (var i = 0; i < 10; i = i + 1) { }");
    test("LOOPS", "Loop infinito", "loop { break; }");
    test("LOOPS", "Foreach", "var arr = [1, 2, 3]; foreach (x in arr) { }");
    test("LOOPS", "Break", "while (true) { break; }");
    test("LOOPS", "Continue", "for (var i = 0; i < 10; i = i + 1) { if (i == 5) continue; }");
    
    test("SWITCH", "Switch básico", "var x = 1; switch (x) { case 1:  default:  }");
    test("SWITCH", "Switch múltiplos cases", "var x = 2; switch (x) { case 1:  case 2:  case 3:  default:  }");
    
    // ========================================
    // CLASSES
    // ========================================
    
    test("CLASSES", "Classe vazia", "class Empty { }");
    
    test("CLASSES", "Classe com fields", 
        "class Point { var x, y; }");
    
    test("CLASSES", "Classe com init", 
        R"(
        class Point {
            var x, y;
            def init(x, y) {
                self.x = x;
                self.y = y;
            }
        }
        )");
    
    test("CLASSES", "Classe com métodos",
        R"(
        class Point {
            var x, y;
            def init(x, y) {
                self.x = x;
                self.y = y;
            }
            def distance() {
                return sqrt(self.x * self.x + self.y * self.y);
            }
        }
        )");
    
    test("CLASSES", "Herança",
        R"(
        class Animal {
            var name;
            def init(name) {
                self.name = name;
            }
        }
        class Dog : Animal {
            def bark() {
                print("Woof!");
            }
        }
        )");
    
    test("CLASSES", "Super call",
        R"(
        class Base {
            def greet() {
                return "Hello";
            }
        }
        class Derived : Base {
            def greet() {
                return super.greet() + " World";
            }
        }
        )");
    
    // ========================================
    // PROCESSOS E FIBERS
    // ========================================
    
    test("PROCESSOS", "Process básico",
        R"(
        process myProcess() {
            print("Running");
        }
        )");
    
    test("PROCESSOS", "Process com fiber",
        R"(

        def fiber1() 
        {
          print("Fiber 1");
        }
        def fiber2()
        {
          print("Fiber 2");
        }
        process myProcess() {
            fiber fiber1();
            fiber fiber2();
        }
        )");
    
    test("PROCESSOS", "Yield em fiber",
        R"(

        def my_fiber()
        {
                print("Start");
                yield(2);
                print("End");
        }

        process myProcess() {
            fiber  my_fiber();
        }
        )");
    
    test("PROCESSOS", "Frame",
        R"(
        process myProcess() 
        {
            
                frame;
            
        }
        )");
    
    // ========================================
    // TRY/CATCH/FINALLY
    // ========================================
    
    test("EXCEPTIONS", "Try-catch",
        R"(
        try {
            var x = 1;
        } catch (e) {
            print(e);
        }
        )");
    
    test("EXCEPTIONS", "Try-finally",
        R"(
        try {
            var x = 1;
        } finally {
            print("Cleanup");
        }
        )");
    
    test("EXCEPTIONS", "Try-catch-finally",
        R"(
        try {
            var x = 1;
        } catch (e) {
            print(e);
        } finally {
            print("Cleanup");
        }
        )");
    
    test("EXCEPTIONS", "Throw",
        R"(
        try {
            throw "Error message";
        } catch (e) {
            print(e);
        }
        )");
    
    // ========================================
    // GOTO/GOSUB/LABELS
    // ========================================
    
    test("GOTO", "Label e goto",
        R"(
        start:
        var x = 1;
        if (x == 0) goto start;
        )");
    
    test("GOTO", "Gosub e return",
        R"(
        gosub sub;
        
        sub:
        print("Subroutine");
        return;
        )");
    
    // ========================================
    // FUNÇÕES MATEMÁTICAS
    // ========================================
    
    test("MATH", "sin", "var x = sin(0.5);");
    test("MATH", "cos", "var x = cos(0.5);");
    test("MATH", "tan", "var x = tan(0.5);");
    test("MATH", "atan", "var x = atan(0.5);");
    test("MATH", "atan2", "var x = atan2(1.0, 1.0);");
    test("MATH", "sqrt", "var x = sqrt(16.0);");
    test("MATH", "pow", "var x = pow(2.0, 3.0);");
    test("MATH", "abs", "var x = abs(-5.0);");
    test("MATH", "floor", "var x = floor(3.7);");
    test("MATH", "ceil", "var x = ceil(3.2);");
    test("MATH", "deg", "var x = deg(3.14159);");
    test("MATH", "rad", "var x = rad(180.0);");
    test("MATH", "log", "var x = log(2.718);");
    test("MATH", "exp", "var x = exp(1.0);");
    
    // ========================================
    // OUTRAS FEATURES
    // ========================================
    
    test("MISC", "Print", "print(42);");
    test("MISC", "Print múltiplos", R"(print("Hello", "World", 42);)");
    test("MISC", "Clock", "var t = clock();");
    test("MISC", "Free", "var arr = [1, 2, 3]; free(arr);");
    //test("MISC", "Type", "var t = type(42);");//TODO
    test("MISC", "Comentário linha", "// comentário\nvar x = 1;");
    test("MISC", "Comentário bloco", "/* comentário */ var x = 1;");
    
    // ========================================
    // SCOPES
    // ========================================
    
    test("SCOPES", "Scope aninhado",
        R"(
        {
            var x = 1;
            {
                var y = 2;
                {
                    var z = 3;
                }
            }
        }
        )");
    
    test("SCOPES", "Variável local shadow global",
        R"(
        var x = 1;
        {
            var x = 2;
        }
        )");
    
    // ========================================
    // CASOS COMPLEXOS
    // ========================================
    
    test("COMPLEXO", "Sistema de partículas",
        R"(
        class Particle {
            var x, y, vx, vy;
            var lifetime;
            
            def init(x, y) {
                self.x = x;
                self.y = y;
                self.vx = 0.0;
                self.vy = 0.0;
                self.lifetime = 100;
            }
            
            def update() {
                self.x = self.x + self.vx;
                self.y = self.y + self.vy;
                self.lifetime = self.lifetime - 1;
            }
            
            def isAlive() {
                return self.lifetime > 0;
            }
        }
        )");
    
    test("COMPLEXO", "Fibonacci recursivo",
        R"(
        def fib(n) {
            if (n <= 1) {
                return n;
            }
            return fib(n - 1) + fib(n - 2);
        }
        var result = fib(10);
        )");
    
    test("COMPLEXO", "Quick sort",
        R"(
        def quicksort(arr, low, high) {
            if (low < high) {
                var pivot = arr[high];
                var i = low - 1;
                
                for (var j = low; j < high; j = j + 1) {
                    if (arr[j] < pivot) {
                        i = i + 1;
                        var temp = arr[i];
                        arr[i] = arr[j];
                        arr[j] = temp;
                    }
                }
                
                var temp = arr[i + 1];
                arr[i + 1] = arr[high];
                arr[high] = temp;
                
                quicksort(arr, low, i);
                quicksort(arr, i + 2, high);
            }
        }
        )");
    
    // ========================================
    // TESTES DE ERROS (devem falhar)
    // ========================================
    
    test("ERROS", "Variável não declarada", "x = 10;", false);
    test("ERROS", "Número overflow", "var x = 99999999999999999999999;", false);
    test("ERROS", "String não fechada", R"(var s = "hello)", false);
    test("ERROS", "Parênteses não balanceados", "var x = (1 + 2;", false);
    // test("ERROS", "Expressão muito aninhada", 
    //     std::string("var x = ") + std::string(300, '(') + "1" + std::string(300, ')') + std::string(";"), 
    //     false);
    test("ERROS", "Array muito grande",
        [](){ 
            std::string code = "var arr = [";
            for(int i = 0; i < 300; i++) {
                if(i > 0) code += ", ";
                code += std::to_string(i);
            }
            code += "];";
            return code;
        }().c_str(),
        false);

    // ========================================
    // RESULTADOS
    // ========================================
    
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║            RESULTADOS                  ║" << std::endl;
    std::cout << "╠════════════════════════════════════════╣" << std::endl;
    std::cout << "║  Total:    " << totalTests << std::endl;
    std::cout << "║  Passou:   " << passedTests << " ✓" << std::endl;
    std::cout << "║  Falhou:   " << failedTests << " ✗" << std::endl;
    std::cout << "║  Taxa:     " << (100.0 * passedTests / totalTests) << "%" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    
    if (failedTests == 0)
    {
        std::cout << "\n🎉 TODOS OS TESTES PASSARAM!" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "\n⚠️  ALGUNS TESTES FALHARAM" << std::endl;
        return 1;
    }
}