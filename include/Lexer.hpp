#pragma once

#include "Token.hpp"
#include "Map.hpp"
#include "Stack.hpp"
#include "Vector.hpp"
#include "String.hpp"



class Lexer
{
private:
    friend class Parser;
    String input;
    int start;
    int current;
    int line;
    bool panicMode;
    Vector<Token> tokens;
    HashTable<TokenType> keywords;
    HashTable<u16> functions;
    HashTable<u16> natives;
    HashTable<u16> processes;
    Vector<String> variables;
    Vector<u16> tokenIndex;
    Stack<u8> brackets; // []
    Stack<u8> braces;   // {}
    Stack<u8> parens;   // ()
    int nativeIndex ;
    String programName;

    bool prevTokenMatch(TokenType type);
    bool lastTokenMatch(TokenType type);
    


    void handleIdentifierTypes();
    void handleFunctionDeclaration();
    void handleProcessDeclaration();
    void handleIdentifier();
    void updateTokenTypes();

    char peek();
    char advance();
    bool match(char expected);
    char peekNext();
    char peekAhead(int n);
    char previous();

    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
   

    void identifier();
    void number();
    void string();

    void blockComment();

    void addToken(TokenType type, const String &literal="");

    void Error(String message);


    String extractIdentifier( String &str);


    bool hasFunction(const String &str);
    bool hasNative(const String &str);
    bool hasProcess(const String &str);

    

    

public:
    Lexer();
    ~Lexer();
    void initialize();
    bool Load( String input);
    void scanToken();
    bool ready();
    void clear();
    Vector<Token> process();
    void addNative(const char *name);
};