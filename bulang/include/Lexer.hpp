#pragma once


#include "Config.hpp"
#include "Token.hpp"
#include "Map.hpp"
#include "Vector.hpp"
#include "String.hpp"


class Interpreter;


class Lexer
{
private:
    String input;
    int start;
    int current;
    int line;
    bool panicMode;

    Vector<Token> tokens;
    Map<String, TokenType> keywords;
    Map<String, int> natives;
    Interpreter* interpreter{ nullptr };

    int brackets{ 0 }; // []
    int braces{ 0 };   // {}
    int parens{ 0 };   // ()
    

    char peek();
    char advance();
    bool match(char expected);
    char peekNext();
    char peekAhead(int n);
    char previous();

    Token back();

    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    void identifier();
    void number();
    void singleQuoteString();

    void string();

    void addToken(TokenType type, const String& literal = "");

    void Error(const String &message);
    void Warning(const String &message);

   String extractIdentifier(const String& str);

public:
    Lexer();
    ~Lexer() = default;

    void Initialize(Interpreter *interpreter);
    bool Load(const String& input);
    void Process();
    bool Ready();
    void Clear();
    void AddNative(const String& name);
    Vector<Token> GetTokens();

};