#pragma once

#include "Token.hpp"
#include "Types.hpp"
#include "Map.hpp"
#include "Stack.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Lexer.hpp"

class VirtualMachine;
class Task;
class Process;


class Parser
{
private:
    Lexer lexer;
    VirtualMachine *vm;


    HashTable<u16> functions;
    HashTable<u16> process;
    Vector<Token> tokens;
    


    int current;
    bool panicMode;
    int countBegins;
    int countEnds;

    bool isAtEnd();

    Token advance();
    Token peek();
    Token previous();
    Token lookAhead();

    void Error(const Token &token, const String &message);
    void Error(const String &message);
    void Warning(const String &message);
    void Warning(const Token &token, const String &message);

    bool match(TokenType type);
    bool match(Vector<TokenType> types);

    Token consume(TokenType type, const String &message);

    bool check(TokenType type);
    void synchronize();

    bool abort();

    void primary(bool canAssign);
    void expression(bool canAssign = true);
    void grouping();
    void unary(bool canAssign);
    void call(bool canAssign);

    void term(bool canAssign);
    void factor(bool canAssign);
    void power(bool canAssign);

    void equality(bool canAssign);   // == !=
    void comparison(bool canAssign); // < > <= >=
    void expr_and(bool canAssign);   // && and
    void expr_or(bool canAssign);    // || or
    void expr_xor(bool canAssign);   // xor
    void expr_now(bool canAssign);

    void assignment(bool canAssign);

    void number();
    void string();

    // statmns
    void program();
    void declaration();
    void defDeclaration();
    void functionDeclaration();
    void processDeclaration();
    void statement();
    void block();
    void expressionStatement();
    void printStatement();
    void variableDeclaration();
    void variable(bool canAssign);

    void ifStatement();
    void switchStatement();
    void whileStatement();
    void doWhileStatement();
    void forStatement();
    void loopStatement();
    void breakStatement();
    void continueStatement();
    void returnStatement();
    void frameStatement();
    void callStatement(bool native);
    u8 argumentList(bool canAssign);
    void callProcess();

    u8 makeConstant(const Value &value);
    void emitByte(u8 byte);
    void emitBytes(u8 byte1, u8 byte2);
    void emitReturn();
    void emitConstant(const Value &value);
    void emitLoop(int loopStart);
    int  emitJump(u8 instruction);
    int  jumpSize(u8 instruction);
    void patchJump(int offset);
    void endLoop();
    

    Task *currentTask;
    bool hasReturned;///dummy , but needed 4 now

    void setTask(Task* task);

public:
    Parser();
    void Init(VirtualMachine *vm);
    bool Load(String text);
    bool Process();
    void Clear();
    void Print();
    void addNative(const char *name);
};