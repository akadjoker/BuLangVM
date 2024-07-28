#include "pch.h"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Utils.hpp"

Lexer::Lexer()
{
}

Lexer::~Lexer()
{
  clear();
}

bool Lexer::Load( String input)
{
  if (input.size() == 0)  return false;
  this->input = std::move(input);
  start = 0;
  current = 0;
  line = 1;
  panicMode = false;
  nativeIndex = 0;
  clear();

  return true;
}

bool Lexer::prevTokenMatch(TokenType type)
{
    return tokens.size() > 1 && tokens[tokens.size() - 2].type == type;
}

bool Lexer::lastTokenMatch(TokenType type)
{
    return tokens.size() > 0 && tokens[tokens.size() - 1].type == type;
}

char Lexer::peek()
{
  if (isAtEnd())
    return '\0';
  return input[current];
}

char Lexer::advance()
{
  return input[current++];
}

bool Lexer::match(char expected)
{
  if (isAtEnd())
    return false;
  if (input[current] != expected)
    return false;

  current++;
  return true;
}

char Lexer::peekNext()
{
  if (current + 1 >= (int)input.size())
    return '\0';
  return input[current + 1];
}

char Lexer::peekAhead(int n)
{
  if (current + n >= (int)input.size())
    return '\0';
  return input[current + n];
}

char Lexer::previous()
{
  if (current - 1 < 0)
    return '\0';
  return input[current - 1];
}

bool Lexer::isAtEnd()
{
  return current >= (int)input.size();
}

bool Lexer::isDigit(char c)
{
  return (c >= '0' && c <= '9');
}

bool Lexer::isAlpha(char c)
{
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         c == '_';
}

bool Lexer::isAlphaNumeric(char c)
{
  return isAlpha(c) || isDigit(c);
}

String Lexer::extractIdentifier(String &str)
{
  String result;
  for (size_t i = 0; i < str.size(); i++)
  {
    char c = str[i];
    if (isAlpha(c) || c == '_')
    {
      result += c;
    }
  }
  return result;
}

void Lexer::Error(String message)
{
  panicMode = true;
  String textLine = String(line);
  String text = message + " at line: " + textLine;
  Log(2, text.c_str());
  clear();
}

void Lexer::addNative(const char *name)
{
  nativeIndex++;
  if (natives.contains(name))
  {
     String buffer(name);
     Error("Native already exists: " + buffer);
     return;
  }
  //INFO("Native added: %s %s" , buffer.c_str(),name);
  natives.insert(name, nativeIndex);
}

void Lexer::initialize()
{

  keywords.insert("program", TokenType::PROGRAM);

  keywords.insert("nil", TokenType::NIL);

  keywords.insert("def", TokenType::FUNCTION);
  keywords.insert("process", TokenType::PROCESS);

  keywords.insert("and", TokenType::AND);
  keywords.insert("or", TokenType::OR);
  keywords.insert("not", TokenType::NOT);
  keywords.insert("xor", TokenType::XOR);

  keywords.insert("if", TokenType::IF);
  keywords.insert("else", TokenType::ELSE);
  keywords.insert("elif", TokenType::ELIF);

  keywords.insert("while", TokenType::WHILE);
  keywords.insert("for", TokenType::FOR);
  keywords.insert("do", TokenType::DO);

  keywords.insert("loop", TokenType::LOOP);

  keywords.insert("break", TokenType::BREAK);
  keywords.insert("continue", TokenType::CONTINUE);
  keywords.insert("return", TokenType::RETURN);
  keywords.insert("switch", TokenType::SWITCH);
  keywords.insert("case", TokenType::CASE);
  keywords.insert("default", TokenType::DEFAULT);

  keywords.insert("print", TokenType::PRINT);
  keywords.insert("now", TokenType::NOW);
  keywords.insert("frame", TokenType::FRAME);
  keywords.insert("class", TokenType::CLASS);
  keywords.insert("this", TokenType::THIS);

  keywords.insert("var", TokenType::VAR);
  keywords.insert("true", TokenType::TRUE);
  keywords.insert("false", TokenType::FALSE);
}

bool Lexer::ready()
{

  if (input.size() == 0)
  {
    Log(2, "Input is empty");
    return false;
  }

  if (brackets.size() > 0)
  {
    Log(2, "Brackets not closed");
    return false;
  }
  if (braces.size() > 0)
  {
    Log(2, "Braces not closed");
    return false;
  }
  if (parens.size() > 0)
  {
    Log(2, "Parens not closed");
    return false;
  }
  return panicMode == false;
}

void Lexer::clear()
{
  start = 0;
  current = 0;
  line = 1;
  tokens.clear();
  brackets.clear();
  braces.clear();
  parens.clear();
  functions.clear();
  processes.clear();
  nativeIndex = 0;
  panicMode = false;
  tokenIndex.clear();
  variables.clear();
  programName = "";

}




void Lexer::updateTokenTypes()
{
  for (size_t i = 0; i < variables.size(); i++)
  {

    u16 index = tokenIndex[i];

    if ((tokens[index].type==TokenType::PROGRAM) || 
        (tokens[index].type==TokenType::CLASS) ||
        (tokens[index].type==TokenType::VAR)    ) continue;

    if (hasFunction(variables[i]))
    {
      if (tokens[index].type != TokenType::IDFUNCTION)
      {
           tokens[index].type = TokenType::IDFUNCTION;
      }
        
     } else 
     if (hasProcess(variables[i]))
     {
      if ( tokens[index].type != TokenType::IDPROCESS)
      {
         tokens[index].type = TokenType::IDPROCESS;
      }
     }
   }
  }

Vector<Token> Lexer::process()
{
  if (panicMode)
  {

    tokens.clear();
    return tokens;
  }
  while (!isAtEnd())
  {
    start = current;
    scanToken();
    if (panicMode)
    {
      tokens.clear();
      return tokens;
    }



    if (prevTokenMatch(TokenType::PROGRAM))
    {
        programName = tokens[tokens.size() - 1].lexeme;
    } else 
    if (prevTokenMatch(TokenType::FUNCTION))
    {
        
        tokens[tokens.size() - 1].type = TokenType::IDFUNCTION;
        functions.insert(tokens[tokens.size() - 1].lexeme.c_str(), tokens.size() - 1);
    } else 
    if (prevTokenMatch(TokenType::PROCESS))
    {
       
        tokens[tokens.size() - 1].type = TokenType::IDPROCESS;
        processes.insert(tokens[tokens.size() - 1].lexeme.c_str(), tokens.size() - 1);
    } else    if (tokens[tokens.size() - 1].type == TokenType::IDENTIFIER)
    {
          if (hasFunction(tokens[tokens.size() - 1].lexeme))
          {
            tokens[tokens.size() - 1].type = TokenType::IDFUNCTION;
          } else 
          if (hasProcess(tokens[tokens.size() - 1].lexeme))
          {
            tokens[tokens.size() - 1].type = TokenType::IDPROCESS;
          } else 
          if (hasNative(tokens[tokens.size() - 1].lexeme))
          {
            tokens[tokens.size() - 1].type = TokenType::IDNATIVE;
          } else 
          {
                variables.push_back(tokens[tokens.size() - 1].lexeme);
                tokenIndex.push_back(tokens.size() - 1);
           }
        
    }

  }
  
  updateTokenTypes();

  return tokens;
}

void Lexer::string()
{
  while (peek() != '"' && !isAtEnd())
  {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd())
  {
    Error("Unterminated string");
    return;
  }

  advance();

  // Trim the surrounding quotes.
  String text = input.substr(start +1 , (current - start) - 2);
  addToken(TokenType::STRING, text);
}

void Lexer::number()
{
  String text = "";
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext()))
  {
    advance();
    while (isDigit(peek()))
      advance();
  }

  text = input.substr(start, current - start);
  addToken(TokenType::NUMBER, text);
}

void Lexer::addToken(TokenType type, const String &literal)
{

  if (type == TokenType::LEFT_BRACKET)
    brackets.push(1);
  if (type == TokenType::LEFT_PAREN)
    parens.emplace(1);
  if (type == TokenType::LEFT_BRACE)
    braces.emplace(1);

  if (brackets.size() > 0)
  {
    if (type == TokenType::RIGHT_BRACKET)
    {
      brackets.pop();
    }
  }
  else
  {
    if (type == TokenType::RIGHT_BRACKET)
    {
      Error("Closing to much brackets ']' ");
    }
  }

  if (braces.size() > 0)
  {
    if (type == TokenType::RIGHT_BRACE)
    {
      braces.pop();
    }
  }
  else
  {
    if (type == TokenType::RIGHT_BRACE)
    {
      Error("Closing to much braces '}' ");
    }
  }

  if (parens.size() > 0)
  {
    if (type == TokenType::RIGHT_PAREN)
    {
      parens.pop();
    }
  }
  else
  {
    if (type == TokenType::RIGHT_PAREN)
    {
      Error("Closing to much parens ')' ");
    }
  }

  String text = input.substr(start, current - start);
  Token token = Token(type, std::move(text), std::move(literal), line);
  tokens.push_back(std::move(token));
}

void Lexer::identifier()
{

  while (isAlphaNumeric(peek()))
    advance();

  // std::cout<<" ,"<<peek()<<" ";

  String text = input.substr(start, current - start);


  TokenType type;
  if (keywords.find(text.c_str(), type))
  {
     addToken(type);
  }
  else
  {
    addToken(TokenType::IDENTIFIER, text);
  }
}

bool Lexer::hasFunction(const String &str)
{

  return functions.contains(str.c_str());
}

bool Lexer::hasNative(const String &str)
{

  return natives.contains(str.c_str());
}

bool Lexer::hasProcess(const String &str)
{

  return processes.contains(str.c_str());
}

void Lexer::scanToken()
{
  char c = advance();
  switch (c)
  {
  case '(':
  {
    addToken(TokenType::LEFT_PAREN);
    break;
  }
  case ')':
  {
    addToken(TokenType::RIGHT_PAREN);
    break;
  }
  case '{':
  {
    addToken(TokenType::LEFT_BRACE);
    break;
  }
  case '}':
  {
    addToken(TokenType::RIGHT_BRACE);
    break;
  }
  case '[':
  {

    addToken(TokenType::LEFT_BRACKET);
    break;
  }
  case ']':
  {
    addToken(TokenType::RIGHT_BRACKET);
    break;
  }

  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
  {
    if (match('-'))
      addToken(TokenType::DEC);
    else if (match('='))
      addToken(TokenType::MINUS_EQUAL);
    else
      addToken(TokenType::MINUS);
    break;
  }
  case '+':
  {
    if (match('+'))
      addToken(TokenType::INC);
    else if (match('='))
      addToken(TokenType::PLUS_EQUAL);
    else
      addToken(TokenType::PLUS);

    break;
  }
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case ':':
    addToken(TokenType::COLON);
    break;

  case '^':
    addToken(TokenType::POWER);
    break;

  case '%':
    addToken(TokenType::MOD);
    break;

  case '*':
  {
    if (match('='))
      addToken(TokenType::STAR_EQUAL);
    else
      addToken(TokenType::STAR);
    break;
  }

  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;

  case '/':
    if (match('/'))
    {
      // single line comment
      while (peek() != '\n' && !isAtEnd())
        advance();
    }
    else if (match('*'))
    {
      // multiline comment block
      blockComment();
    }
    else if (match('='))
    {
      addToken(TokenType::SLASH_EQUAL);
    }
    else
    {
      addToken(TokenType::SLASH);
    }
    break;
    break;

  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace.
    break;

  case '\n':
    line++;
    break;
  case '"':
    string();
    break;

  default:
    if (isDigit(c))
    {
      number();
    }
    else if (isAlpha(c))
    {
      identifier();
    }
    else
    {
      Error("Unexpected character");
    }

    break;
  }
}

void Lexer::blockComment()
{
  while (!isAtEnd())
  {
    if (peek() == '*' && peekNext() == '/')
    {
      advance();
      advance();
      return;
    }
    if (peek() == '\n')
    {
      line++;
    }
    advance();
  }
  Error("Unterminated comment");
}


