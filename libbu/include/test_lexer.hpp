#pragma once

#include <cassert>
#include <vector>
#include "lexer.hpp"
#include "token.hpp"

class TestLexer
{
public:
    static void runAllTests()
    {
        testConstructors();
        testScanToken();
        testNextToken();
        testScanAll();
        testIsKeyword();
        testReset();
        testWhitespace();
        testNumbers();
        testStrings();
        testIdentifiers();
    }

private:
    static void testConstructors()
    {
        Lexer lex1("int x = 42;");
        assert(lex1.scanToken().type != TokenType::END_OF_FILE);
        
        const char* src = "x";
        Lexer lex2(src, 1);
        assert(lex2.scanToken().type != TokenType::END_OF_FILE);
    }

    static void testScanToken()
    {
        Lexer lex("x");
        Token tok = lex.scanToken();
        assert(!tok.lexeme.empty());
    }

    static void testNextToken()
    {
        Lexer lex("x y");
        Token tok1 = lex.nextToken();
        Token tok2 = lex.nextToken();
        assert(tok1.lexeme != tok2.lexeme);
    }

    static void testScanAll()
    {
        Lexer lex("int x;");
        std::vector<Token> tokens = lex.scanAll();
        assert(!tokens.empty());
    }

    static void testIsKeyword()
    {
        Lexer lex("");
        assert(lex.isKeyword("int") == true);
        assert(lex.isKeyword("myvar") == false);
    }

    static void testReset()
    {
        Lexer lex("test");
        lex.scanToken();
        lex.reset();
        Token tok = lex.scanToken();
        assert(!tok.lexeme.empty());
    }

    static void testWhitespace()
    {
        Lexer lex("x   y");
        std::vector<Token> tokens = lex.scanAll();
        assert(tokens.size() >= 2);
    }

    static void testNumbers()
    {
        Lexer lex("42 3.14");
        std::vector<Token> tokens = lex.scanAll();
        assert(tokens.size() >= 2);
    }

    static void testStrings()
    {
        Lexer lex("\"hello\"");
        Token tok = lex.scanToken();
        assert(tok.type != TokenType::END_OF_FILE);
    }

    static void testIdentifiers()
    {
        Lexer lex("variable_name");
        Token tok = lex.scanToken();
        assert(tok.lexeme == "variable_name");
    }
};