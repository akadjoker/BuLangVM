#pragma once


#include "Config.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Mem.hpp"
#include "Ast.hpp"

class Interpreter;
class Lexer;
class Compiler;


   
class Parser
{
public:
    Parser(Interpreter* interpreter);
    ~Parser();

    bool Load(const String& source);

    bool  Parse(ByteGenerator* visitor);

    void Clear();

private:

    Interpreter* interpreter{ nullptr };
    
    
    Vector<Token> tokens;
    int current;
    bool panicMode;
    int countBegins;
    int countEnds;
    Token tokenError;
    bool callReturn{ false };
    
 



    bool match(TokenType type);
    Token consume(TokenType type, const String& message);
    bool check(TokenType type);
    bool isAtEnd();
    void synchronize();
    Token advance();
    Token peek();
    Token previous();
    Token lookAhead();

    void Error(const Token& token, const String& message);
    void Error(const String& message);
    void Warning(const Token& token, const String& message);
    void Warning( const String& message);

    NodePtr NONE;

    //the real parser
    //Expressions
    
    NodePtr number();
    NodePtr string();
    NodePtr nil();


    NodePtr call(bool canAssign);

    NodePtr callStatement(bool native);

    NodePtr assignment(bool canAssign);
    NodePtr primary(bool canAssign);
    NodePtr expression(bool canAssign = true);
    NodePtr grouping();
    NodePtr unary(bool canAssign);


    NodePtr term(bool canAssign);
    NodePtr factor(bool canAssign);
    NodePtr expr_and(bool canAssign);   // && and
    NodePtr expr_or(bool canAssign);    // || or
    NodePtr expr_xor(bool canAssign);   // xor


    NodePtr equality(bool canAssign);   // == !=
    NodePtr comparison(bool canAssign); // < > <= >=

    Vector<NodePtr> argumentList(bool canAssign);

    // Statements

    NodePtr variable_declaration(bool canAssign=false);
    NodePtr variable(bool canAssign=false);


    NodePtr statement();
    NodePtr expression_statement(bool canAssign=true);
    NodePtr declaration();
    NodePtr block();
    NodePtr print_statement();
    NodePtr now_statement();
    
    NodePtr if_statement();
    NodePtr switch_statement();
    
    
    NodePtr label_statement();
    NodePtr goto_statement();
    NodePtr gosub_statement();
    
    NodePtr while_statement();
    NodePtr do_statement();
    NodePtr for_statement();


    NodePtr return_statement();

    NodePtr break_statement();
    NodePtr continue_statement();


    NodePtr function_declaration();
    NodePtr class_declaration();
    NodePtr struct_declaration();



};