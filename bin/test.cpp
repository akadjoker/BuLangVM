
#include "pch.h"


#include "Config.hpp"
#include "Utils.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Raii.hpp"
#include "Map.hpp"
#include "Stack.hpp"
#include "Types.hpp"
#include "Vm.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"


#include <iostream>
#include <unordered_map>
#include <chrono>
#include <string>

inline void PrintValue(Value value)
{
    if (IS_STRING(value))
        printf("%s\n", AS_STRING(value)->string.c_str());
    else if (IS_NUMBER(value))
        printf("%f\n", AS_NUMBER(value));
    else if (IS_BOOLEAN(value))
        printf("%s\n", AS_BOOLEAN(value) ? "true" : "false");
    else if (IS_NONE(value))
        printf("none");
}        


static Value native_say(VirtualMachine* vm, int argc, Value* args)
{
    for (int i = 0; i < argc; i++)
    {
        PrintValue(args[i]);
    }
    return BOOLEAN(true);
}

static Value native_clock(VirtualMachine* vm, int argc, Value* args)
{
    return NUMBER( (double)clock() / CLOCKS_PER_SEC);
}



bool StressTest(const char* text, Value expected)
{
    VirtualMachine vm;
    Lexer lexer;
    Parser parser(&vm);
    if (lexer.Load(text))
    {
        Vector<Token> tokens = lexer.scanTokens();
        
        if (parser.Load(tokens))
        {
            parser.Process();
            vm.Run();
            Value result = vm.top();
            if (MatchValue(result, expected))
            {
                INFO("Stress test (%s) passed", text);
                return true;
            } else 
            {
                WARNING("Stress test (%s) failed", text);
                printf("        Expected: ");
                PrintValue(expected);
                printf("        Got: ");
                PrintValue(result); 
                return false;
            }
        }
    }
    WARNING("Stress test (%s) failed", text);
    return false;
}

bool RunMachine(const String &text)
{
    VirtualMachine vm;
    Lexer lexer;
    lexer.addNative("say");
    lexer.addNative("clock");
    vm.registerFunction("say", native_say, -1);
    vm.registerFunction("clock", native_clock, 0);
    Parser parser(&vm);
    if (lexer.Load(text.c_str()))
    {
        Vector<Token> tokens = lexer.scanTokens();

    //          int line = -1;
    // for (size_t i = 0; i < tokens.size(); i++)
    // {
    //     Token token = tokens[i];
    //     if (token.line != line)
    //     {
    //         line = token.line;
    //         printf("%4d " ,line);
    //     } else 
    //     {
    //         printf("   | ");
    //     }
    //     if (token.type == TokenType::IDFUNCTION)
    //     {
    //            printf("DEF '%s' \n",  token.lexeme.c_str());
    //     } else if (token.type == TokenType::IDNATIVE)
    //     {
    //             printf("NATIVE '%s' \n",  token.lexeme.c_str());
    //     } else if (token.type == TokenType::PROCESS)
    //     {
    //         printf("PROCESS '%s' \n",  token.lexeme.c_str());
    //     } else 
    //     {
    //         printf("%2d '%.*s' \n", (int)token.type, (int)token.lexeme.size(), token.lexeme.c_str());
    //     }
    // }
        
        if (parser.Load(tokens))
        {
            parser.Process();
            vm.Compile();
            vm.Run();
            vm.PrintStack();

            Arena::as().stats();
           // Value result = vm.top();
          // PrintValue(result);
            return true;
        }
    }

    return false;
            
}

const int MAX_TEST = 100000;
const int numIterations = 10;  // Número de iterações para calcular a média


void runTableTest() 
{
     double totalInsertTime = 0;
    double totalFindTime = 0;
    double totalEraseTime = 0;

    for (int j = 0; j < numIterations; ++j) 
    {
    Table table;

    // Teste de inserção
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) 
    {
        String key("key");
        key += String(i);
        table.set(key, NUMBER(static_cast<double>(i)));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    
    totalInsertTime += duration.count();

    // Teste de busca
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {
        
        String key("key");
        key += String(i);
        Value value;
        table.get(key, &value);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    totalFindTime += duration.count();

    // Teste de remoção
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {

        String key("key");
        key += String(i);
        table.remove(key);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    totalEraseTime += duration.count();

    }
    printf("Insert time: %f\n", totalInsertTime / numIterations);
    printf("Find time: %f\n", totalFindTime / numIterations);
    printf("Erase time: %f\n", totalEraseTime / numIterations);
}

void runMapTest() 
{
     double totalInsertTime = 0;
    double totalFindTime = 0;
    double totalEraseTime = 0;


    for (int i = 0; i < numIterations; ++i)
    {

    
    HashTable<Value> table;

    // Teste de inserção
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) 
    {
         std::string key = "key" + std::to_string(i);
        table.insert(key.c_str(), NUMBER(static_cast<double>(i)));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    totalInsertTime += duration.count();

    // Teste de busca
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {
        
        std::string key = "key" + std::to_string(i);
        Value value;
        table.find(key.c_str(), value);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    totalFindTime += duration.count();

    // Teste de remoção
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {

        std::string key = "key" + std::to_string(i);
        table.erase(key.c_str());
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    totalEraseTime += duration.count();
    }

    std::cout << "Map Inserção: " << totalInsertTime / numIterations << " segundos\n";
    std::cout << "Map Busca: " << totalFindTime / numIterations << " segundos\n";
    std::cout << "Map Remoção: " << totalEraseTime / numIterations << " segundos\n";    
}
void runUnorderedMapTest() 
{
     double totalInsertTime = 0;
    double totalFindTime = 0;
    double totalEraseTime = 0;

    for (int j = 0; j < numIterations; ++j) 
    {
    std::unordered_map<std::string, Value> unorderedMap;

    // Teste de inserção
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {
        std::string key = "key" + std::to_string(i);
        unorderedMap[key] = NUMBER(static_cast<double>(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    totalInsertTime += duration.count();

    // Teste de busca
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {
        std::string key = "key" + std::to_string(i);
        auto it = unorderedMap.find(key);
        if (it != unorderedMap.end()) 
        {
            
        }
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    totalFindTime += duration.count();

    // Teste de remoção
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_TEST; ++i) {
        std::string key = "key" + std::to_string(i);
        unorderedMap.erase(key);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    totalEraseTime += duration.count();
    }
    std::cout << "UnorderedMap Inserção: " << totalInsertTime / numIterations << " segundos\n";
    std::cout << "UnorderedMap Busca: " << totalFindTime / numIterations << " segundos\n";
    std::cout << "UnorderedMap Remoção: " << totalEraseTime / numIterations << " segundos\n";
}
int main()
{
    bool stessTest = false;
    if (stessTest)
    {

    PRINT("simples teste");
    StressTest("1 + 1", NUMBER(2));
    StressTest("(1 + 1) * 2", NUMBER(4));
    StressTest("1 + 1 * 2", NUMBER(3));
    StressTest("1 + (1 * 2)", NUMBER(3));
    StressTest("1 + 1 * 2 + 1", NUMBER(4));
    StressTest("1 + (1 * 2) + 1", NUMBER(4));


    PRINT("Testing precedence of operations");

    StressTest("3 + 2 * 5", NUMBER(13));
    StressTest("3 * 2 ^ 2", NUMBER(12));
    StressTest("(3 + 2) * 5", NUMBER(25));
    StressTest("(3 * 2) ^ 2", NUMBER(36));


    PRINT("Testing float numbers");
    StressTest("3.2 + 2.8", NUMBER(6.0));
    StressTest("5.5 * 2", NUMBER(11.0));
    StressTest("6.25 / 2.5", NUMBER(2.5));
    StressTest("8.9 - 2.9", NUMBER(6.0));
    StressTest("10.75 % 3.5", NUMBER(0.25));


    PRINT("Testing negative numbers");
    StressTest("-2 + 3", NUMBER(1));
    StressTest("3 * -2", NUMBER(-6));
    StressTest("-2 ^ 3", NUMBER(-8));
    StressTest("10 / -2", NUMBER(-5));
    StressTest("-10 - -5", NUMBER(-5));
    StressTest("10 % -3", NUMBER(-2));


    PRINT("Testing nested expressions");
    StressTest("(3 + (2 * 5)) - (2 * (2 + 3))", NUMBER(3));
    StressTest("((3 ^ 2) - 4) * 2", NUMBER(10));
    StressTest("((12 / 2) + 5) * 2", NUMBER(22));
    StressTest("((7 + 3) % 4) * 5", NUMBER(10));
    StressTest("((2 ^ (2 + 1)) * 2)", NUMBER(16));


    PRINT("Testing precedence of operators");
    StressTest("((1 + 2) * 3) - (4 / 2)", NUMBER(7));
    StressTest("3 + 4 * 2 / (1 - 5) ^ 2 ^ 3", NUMBER(3));  // Dependendo da implementação específica de precedência de `^`
    StressTest("true and (false or true)", BOOLEAN(true));
    StressTest("true or (false and true)", BOOLEAN(true));

    PRINT("Operadores unários");
    StressTest("-5", NUMBER(-5));
    StressTest("!true", BOOLEAN(false));

    PRINT("Operadores lógicos");
    StressTest("true and false", BOOLEAN(false));
    StressTest("true or false", BOOLEAN(true));
    StressTest("!true", BOOLEAN(false));
    StressTest("!false", BOOLEAN(true));

    PRINT("Operadores relacionais");
    StressTest("3 > 2", BOOLEAN(true));
    StressTest("3 < 2", BOOLEAN(false));
    StressTest("3 >= 3", BOOLEAN(true));
    StressTest("3 <= 2", BOOLEAN(false));
    StressTest("3 == 3", BOOLEAN(true));
    StressTest("3 != 2", BOOLEAN(true));
    }


    char *text = LoadFileText("main.pc");
    if (!text)
    {
        ERROR("Failed to load main.pc");
        return 1;
    }
    String str(text);
    FreeFileText(text);

//    RunMachine(str);

   

    std::cout << "\nTestando UnorderedMap:\n";
    runUnorderedMapTest();

    std::cout << "\nTestando Map:\n";
    runMapTest();
    return 0;
}