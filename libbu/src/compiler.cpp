#include "compiler.hpp"
#include "code.hpp"
#include "interpreter.hpp"
#include "platform.hpp"
#include "opcode.hpp"
#include "pool.hpp"
#include "value.hpp"
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>

// ============================================
// PARSE RULE TABLE - DEFINIÇÃO
// ============================================
constexpr ParseRule makeRule(ParseFn prefix, ParseFn infix, Precedence prec)
{
  return {prefix, infix, prec};
}

ParseRule Compiler::rules[TOKEN_COUNT];

// ============================================
// CONSTRUCTOR
// ============================================

Compiler::Compiler(Interpreter *vm)
    : vm_(vm), lexer(nullptr), function(nullptr), currentChunk(nullptr),
      currentFiber(nullptr), currentProcess(nullptr), hadError(false),
      panicMode(false), scopeDepth(0), localCount_(0), loopDepth_(0),
      isProcess_(false)
{

  initRules();
  cursor = 0;
}
Compiler::~Compiler()
{
 
  delete lexer; 
}

// ============================================
// INICIALIZAÇÃO DA TABELA
// ============================================

void Compiler::initRules()
{

  for (int i = 0; i < TOKEN_COUNT; i++)
  {
    rules[i] = {nullptr, nullptr, PREC_NONE};
  }

  // Agora define os que têm funções
  rules[TOKEN_LPAREN] = {&Compiler::grouping, &Compiler::call, PREC_CALL};
  rules[TOKEN_RPAREN] = {nullptr, nullptr, PREC_NONE};
  rules[TOKEN_LBRACE] = {nullptr, nullptr, PREC_NONE};
  rules[TOKEN_RBRACE] = {nullptr, nullptr, PREC_NONE};
  rules[TOKEN_COMMA] = {nullptr, nullptr, PREC_NONE};
  rules[TOKEN_SEMICOLON] = {nullptr, nullptr, PREC_NONE};

  rules[TOKEN_DOT] = {nullptr, &Compiler::dot, PREC_CALL};
  rules[TOKEN_SELF] = {&Compiler::self, nullptr, PREC_NONE};
  rules[TOKEN_SUPER] = {&Compiler::super, nullptr, PREC_NONE};

  // Arithmetic
  rules[TOKEN_PLUS] = {nullptr, &Compiler::binary, PREC_TERM};
  rules[TOKEN_MINUS] = {&Compiler::unary, &Compiler::binary, PREC_TERM};
  rules[TOKEN_STAR] = {nullptr, &Compiler::binary, PREC_FACTOR};
  rules[TOKEN_SLASH] = {nullptr, &Compiler::binary, PREC_FACTOR};
  rules[TOKEN_PERCENT] = {nullptr, &Compiler::binary, PREC_FACTOR};

  // Comparison
  rules[TOKEN_EQUAL] = {nullptr, nullptr, PREC_NONE};
  rules[TOKEN_EQUAL_EQUAL] = {nullptr, &Compiler::binary, PREC_EQUALITY};
  rules[TOKEN_BANG_EQUAL] = {nullptr, &Compiler::binary, PREC_EQUALITY};
  rules[TOKEN_LESS] = {nullptr, &Compiler::binary, PREC_COMPARISON};
  rules[TOKEN_LESS_EQUAL] = {nullptr, &Compiler::binary, PREC_COMPARISON};
  rules[TOKEN_GREATER] = {nullptr, &Compiler::binary, PREC_COMPARISON};
  rules[TOKEN_GREATER_EQUAL] = {nullptr, &Compiler::binary, PREC_COMPARISON};

  rules[TOKEN_PLUS_PLUS] = {&Compiler::prefixIncrement, nullptr, PREC_NONE};
  rules[TOKEN_MINUS_MINUS] = {&Compiler::prefixDecrement, nullptr, PREC_NONE};

  // Logical
  rules[TOKEN_AND_AND] = {nullptr, &Compiler::and_, PREC_AND};
  rules[TOKEN_OR_OR] = {nullptr, &Compiler::or_, PREC_OR};
  rules[TOKEN_BANG] = {&Compiler::unary, nullptr, PREC_NONE};

  rules[TOKEN_PIPE] = {nullptr, &Compiler::binary, PREC_BITWISE_OR};
  rules[TOKEN_CARET] = {nullptr, &Compiler::binary, PREC_BITWISE_XOR};
  rules[TOKEN_AMPERSAND] = {nullptr, &Compiler::binary, PREC_BITWISE_AND};
  rules[TOKEN_TILDE] = {&Compiler::unary, nullptr, PREC_NONE};
  rules[TOKEN_LEFT_SHIFT] = {nullptr, &Compiler::binary, PREC_SHIFT};
  rules[TOKEN_RIGHT_SHIFT] = {nullptr, &Compiler::binary, PREC_SHIFT};

  // Literals
  rules[TOKEN_INT] = {&Compiler::number, nullptr, PREC_NONE};
  rules[TOKEN_FLOAT] = {&Compiler::number, nullptr, PREC_NONE};
  rules[TOKEN_STRING] = {&Compiler::string, nullptr, PREC_NONE};
  rules[TOKEN_IDENTIFIER] = {&Compiler::variable, nullptr, PREC_NONE};
  rules[TOKEN_TRUE] = {&Compiler::literal, nullptr, PREC_NONE};
  rules[TOKEN_FALSE] = {&Compiler::literal, nullptr, PREC_NONE};
  rules[TOKEN_NIL] = {&Compiler::literal, nullptr, PREC_NONE};

  rules[TOKEN_FOREACH] = {nullptr, nullptr, PREC_NONE};


  rules[TOKEN_LBRACKET] = {
      &Compiler::arrayLiteral, //  PREFIX: [1, 2, 3]
      &Compiler::subscript,    //  INFIX: arr[i]
      PREC_CALL                //  Mesma precedência que . e ()
  };

  rules[TOKEN_LEN] = {
    &Compiler::lengthExpression, // PREFIX
    nullptr,                     // INFIX
    PREC_NONE
};

  rules[TOKEN_LBRACE] = {&Compiler::mapLiteral, //  PREFIX: {key: value}
                         nullptr,               //  Sem INFIX
                         PREC_NONE};

  // Keywords (all nullptr já setados no loop)
  rules[TOKEN_EOF] = {nullptr, nullptr, PREC_NONE};
  rules[TOKEN_ERROR] = {nullptr, nullptr, PREC_NONE};
}

// ============================================
// MAIN ENTRY POINT
// ============================================

void Compiler::setFileLoader(FileLoaderCallback loader, void *userdata)
{
  fileLoader = loader;
  fileLoaderUserdata = userdata;
}

ProcessDef *Compiler::compile(const std::string &source)
{

  lexer = new Lexer(source);
  tokens = lexer->scanAll();
  if (!tokens.size())
  {

    error("Empty source");
    return nullptr;
  }

  function = vm_->addFunction("__main__", 0);
  if (!function)
  {
    error("Fail to create main function");
    return nullptr;
  }
  currentProcess = vm_->addProcess("__main_process__", function);
  if (!currentProcess)
  {
    error("Fail to create main process");
    return nullptr;
  }
  currentChunk = function->chunk;
  currentFiber = &currentProcess->fibers[0];
  currentFunctionType = FunctionType::TYPE_SCRIPT;
  currentClass = nullptr;

  advance();

  while (!match(TOKEN_EOF) && !hadError)
  {
    declaration();
  }

  emitReturn();

  if (hadError)
  {
    return nullptr;
  }

  currentProcess->finalize();

  importedModules.clear();
  usingModules.clear();;

  return currentProcess;
}

ProcessDef *Compiler::compileExpression(const std::string &source)
{

  lexer = new Lexer(source);

  tokens = lexer->scanAll();

  function = vm_->addFunction("__expr__", 0);
  currentProcess = vm_->addProcess("__main__", function);
  currentChunk = function->chunk;
  currentFiber = &currentProcess->fibers[0];
  currentFunctionType = FunctionType::TYPE_SCRIPT;
  currentClass = nullptr;

  advance();

  if (check(TOKEN_EOF))
  {
    error("Empty expression");
    function = nullptr;
    return nullptr;
  }

  expression();
  consume(TOKEN_EOF, "Expect end of expression");

  emitByte(OP_RETURN);

  if (hadError)
  {
    return nullptr;
  }
  currentProcess->finalize();

  importedModules.clear();
  usingModules.clear();


  return currentProcess;
}

void Compiler::clear()
{
  delete lexer;
  tokens.clear();

  lexer = nullptr;
  function = nullptr;
  currentChunk = nullptr;
  currentFiber = nullptr;
  currentProcess = nullptr;
  currentClass = nullptr;
  hadError = false;
  importedModules.clear();
  usingModules.clear();

  panicMode = false;
  scopeDepth = 0;
  localCount_ = 0;
  loopDepth_ = 0;
  cursor = 0;
  currentFunctionType = FunctionType::TYPE_SCRIPT;
}

// ============================================
// TOKEN MANAGEMENT
// ============================================

void Compiler::advance()
{

  previous = current;
  if (cursor >= (int)tokens.size())
  {
    current.type = TOKEN_EOF;
    return;
  }

  current = tokens[cursor++];
}

Token Compiler::peek(int offset)
{
  size_t index = cursor + offset;
  if (index >= tokens.size())
  {
    return tokens.back(); // EOF
  }
  return tokens[index];
}

bool Compiler::checkNext(TokenType t) { return peek(0).type == t; }

bool Compiler::check(TokenType type) { return current.type == type; }

bool Compiler::match(TokenType type)
{
  if (!check(type))
    return false;
  advance();
  return true;
}

void Compiler::consume(TokenType type, const char *message)
{
  if (current.type == type)
  {
    advance();
    return;
  }

  errorAtCurrent(message);
}

// ============================================
// ERROR HANDLING
// ============================================

void Compiler::fail(const char *fmt, ...)
{
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  errorAt(previous, buffer);
}

void Compiler::error(const char *message) { errorAt(previous, message); }

void Compiler::errorAt(Token &token, const char *message)
{
  if (panicMode)
    return;
  panicMode = true;

  OsPrintf("[line %d] Error", token.line);

  if (token.type == TOKEN_EOF)
  {
    OsPrintf(" at end");
  }
  else if (token.type == TOKEN_ERROR)
  {
    // Nothing
  }
  else
  {
    OsPrintf(" at '%s'", token.lexeme.c_str());
  }

  OsPrintf(": %s\n", message);
  hadError = true;
}

void Compiler::errorAtCurrent(const char *message)
{
  errorAt(current, message);
}

void Compiler::synchronize()
{
    panicMode = false;
    
    while (current.type != TOKEN_EOF)
    {
        if (previous.type == TOKEN_SEMICOLON)
            return;
        
        switch (current.type)
        {
            //  TOP-LEVEL DECLARATIONS
            case TOKEN_IMPORT:
            case TOKEN_USING:
            case TOKEN_INCLUDE:
            case TOKEN_DEF:
            case TOKEN_PROCESS:
            case TOKEN_CLASS:
            case TOKEN_STRUCT:
            case TOKEN_VAR:
            
            //  CONTROL FLOW STATEMENTS
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_DO:
            case TOKEN_LOOP:
            case TOKEN_FOR:
            case TOKEN_SWITCH:
            
            //  JUMP STATEMENTS
            case TOKEN_BREAK:
            case TOKEN_CONTINUE:
            case TOKEN_RETURN:
            case TOKEN_GOTO:
            case TOKEN_GOSUB:
            
            //  SPECIAL STATEMENTS
            case TOKEN_PRINT:
            case TOKEN_YIELD:
            case TOKEN_FIBER:
            case TOKEN_FRAME:
            case TOKEN_EXIT:
                return;
            
            default:
                ; // Nothing
        }
        
        advance();
    }
}

// ============================================
// BYTECODE EMISSION
// ============================================

void Compiler::emitByte(uint8 byte)
{
  currentChunk->write(byte, previous.line);
}

void Compiler::emitBytes(uint8 byte1, uint8 byte2)
{
  emitByte(byte1);
  emitByte(byte2);
}

void Compiler::emitReturn()
{
  emitByte(OP_NIL);
  emitByte(OP_RETURN);
}

void Compiler::emitConstant(Value value)
{
  emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8 Compiler::makeConstant(Value value)
{

  int constant = currentChunk->addConstant(value);
  if (constant > UINT8_MAX)
  {
    error("Too many constants in one chunk");
    return 0;
  }

  return (uint8)constant;
}

// ============================================
// JUMPS
// ============================================

int Compiler::emitJump(uint8 instruction)
{
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk->count - 2;
}

void Compiler::patchJump(int offset)
{
  int jump = currentChunk->count - offset - 2;

  if (jump > UINT16_MAX)
  {
    error("Too much code to jump over");
  }

  currentChunk->code[offset] = (jump >> 8) & 0xff;
  currentChunk->code[offset + 1] = jump & 0xff;
}

void Compiler::emitLoop(int loopStart)
{
  emitByte(OP_LOOP);

  int offset = currentChunk->count - loopStart + 2;
  if (offset > UINT16_MAX)
  {
    error("Loop body too large");
  }

  emitByte((offset >> 8) & 0xff);
  emitByte(offset & 0xff);
}

void Compiler::patchJumpTo(int operandOffset, int targetOffset)
{
  int jump = targetOffset - (operandOffset + 2); // target - after operand
  if (jump < 0)
  {
    error("Backward goto must use OP_LOOP");
    return;
  }
  if (jump > UINT16_MAX)
  {
    error("Jump distance too large");
    return;
  }

  currentChunk->code[operandOffset] = (jump >> 8) & 0xff;
  currentChunk->code[operandOffset + 1] = jump & 0xff;
}

void Compiler::emitGosubTo(int targetOffset)
{
  emitByte(OP_GOSUB);
  int from = (int)currentChunk->count + 2;
  int delta = targetOffset - from;
  if (delta < -32768 || delta > 32767)
  {
    error("gosub jump out of range");
  }

  emitByte((delta >> 8) & 0xff);
  emitByte(delta & 0xff);
}

// ============================================
// PRATT PARSER - CORE
// ============================================

void Compiler::parsePrecedence(Precedence precedence)
{
  advance();

  ParseFn prefixRule = getRule(previous.type)->prefix;

  if (prefixRule == nullptr)
  {
    error("Expect expression");
    return;
  }

  bool canAssign = (precedence <= PREC_ASSIGNMENT);
  (this->*prefixRule)(canAssign);

  while (precedence <= getRule(current.type)->prec)
  {
    advance();
    ParseFn infixRule = getRule(previous.type)->infix;

    if (infixRule == nullptr)
    {
      break;
    }
    (this->*infixRule)(canAssign);
  }

  if (canAssign && match(TOKEN_EQUAL))
  {
    error("Invalid assignment target");
  }
}
ParseRule *Compiler::getRule(TokenType type) { return &rules[type]; }

void Compiler::resolveGotos()
{
  for (const GotoJump &jump : pendingGotos)
  {
    int targetOffset = -1;

    for (const Label &l : labels)
    {
      if (l.name == jump.target)
      {
        targetOffset = l.offset;
        break;
      }
    }

    if (targetOffset == -1)
    {
      error("Undefined label");
      continue;
    }

    patchJumpTo(jump.jumpOffset, targetOffset);
  }

  pendingGotos.clear();
}

void Compiler::resolveGosubs()
{
  for (const auto &j : pendingGosubs)
  {
    int targetOffset = -1;
    for (const auto &l : labels)
      if (l.name == j.target)
      {
        targetOffset = l.offset;
        break;
      }

    if (targetOffset < 0)
    {
      error("Undefined label");
      continue;
    }

    patchJumpTo(j.jumpOffset, targetOffset); // signed int16
  }
  pendingGosubs.clear();
}
