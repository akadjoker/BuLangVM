// ============================================
// TESTES DE EARLY EXIT - Parser deve parar logo
// ============================================

#include "compiler.hpp"
#include "interpreter.hpp"
#include <iostream>

void testEarlyExit(const char* testName, const char* code, int expectedMaxErrors = 1)
{
    Interpreter vm;
   
    
    Compiler compiler(&vm);
    ProcessDef* result = compiler.compile(code);
    
    auto stats = compiler.getStats();
    
    std::cout << "Test: " << testName << std::endl;
    std::cout << "  Errors: " << stats.totalErrors << std::endl;
    std::cout << "  Expected max: " << expectedMaxErrors << std::endl;
    
    if (result != nullptr)
    {
        std::cout << "  ✗ FALHOU - Devia ter falhado compilação" << std::endl;
    }
    else if (stats.totalErrors > expectedMaxErrors)
    {
        std::cout << "  ✗ FALHOU - Muitos erros em cascata!" << std::endl;
    }
    else
    {
        std::cout << "  ✓ PASSOU - Parou logo no erro" << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    std::cout << "╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║   TESTES DE EARLY EXIT DO PARSER      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // ========================================
    // 1. ERROS EM EXPRESSÕES
    // ========================================
    
    testEarlyExit("Expressão inválida",
        "var x = + 5;",  // Operador + sem operando esquerdo
        1);
    
    testEarlyExit("Parênteses não fechados",
        "var x = (1 + 2;",
        1);
    
    testEarlyExit("Array não fechado",
        "var arr = [1, 2, 3;",
        1);
    
    testEarlyExit("Map não fechado",
        "var m = {a: 1, b: 2;",
        1);

    // ========================================
    // 2. ERROS EM NÚMEROS
    // ========================================
    
    testEarlyExit("Número overflow",
        "var x = 99999999999999999999999999;",
        1);
    
    testEarlyExit("Float overflow",
        "var x = 1e9999;",
        1);
    
    testEarlyExit("Hex inválido",
        "var x = 0xGGGG;",
        1);

    // ========================================
    // 3. ERROS EM DECLARATIONS
    // ========================================
    
    testEarlyExit("Variável sem nome",
        "var = 10;",
        1);
    
    testEarlyExit("Função sem nome",
        "def () { return 1; }",
        1);
    
    testEarlyExit("Função sem parênteses",
        "def foo { return 1; }",
        1);

    // ========================================
    // 4. ERROS EM BLOCOS
    // ========================================
    
    testEarlyExit("If sem condição",
        "if { var x = 1; }",
        1);
    
    testEarlyExit("While sem condição",
        "while { var x = 1; }",
        1);
    
    testEarlyExit("For mal formado",
        "for var i = 0 { }",
        2);  // Pode gerar 2 erros (falta ; e condição)

    // ========================================
    // 5. ERROS EM CLASSES
    // ========================================
    
    testEarlyExit("Classe sem nome",
        "class { var x; }",
        1);
    
    testEarlyExit("Método sem nome",
        "class Foo { def () { } }",
        1);

    // ========================================
    // 6. ERROS DE PROFUNDIDADE
    // ========================================
    
    // Criar código com expressões muito aninhadas
    std::string deepExpr = "var x = ";
    for (int i = 0; i < 250; i++) {
        deepExpr += "(";
    }
    deepExpr += "1";
    for (int i = 0; i < 250; i++) {
        deepExpr += ")";
    }
    deepExpr += ";";
    
    testEarlyExit("Expressão muito aninhada",
        deepExpr.c_str(),
        1);

    // ========================================
    // 7. CÓDIGO COMPLEXO (caso real)
    // ========================================
    
    testEarlyExit("Código com erro simples não deve gerar cascata",
        R"(
            class Particle {
                var x, y;
                var vx, vy;
                
                def init(x, y) {
                    self.x = x
                    self.y = y;  // <- Falta ; na linha anterior
                    self.vx = 0;
                    self.vy = 0;
                }
                
                def update() {
                    self.x += self.vx;
                    self.y += self.vy;
                }
            }
        )",
        1);  // Só 1 erro (falta ;), não deve gerar cascata

    // ========================================
    // 8. VERIFICAR hadError PARA LOGO
    // ========================================
    
    testEarlyExit("makeConstant com hadError deve parar",
        R"(
            var a = 1;
            var b = +;  // Erro aqui
            var c = 2;  // Não deve tentar compilar isto
        )",
        1);

    std::cout << "╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║         TESTES CONCLUÍDOS              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    return 0;
}