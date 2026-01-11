#pragma once

#include "config.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include "types.hpp"
#include "vector.hpp"
#include "set.hpp"
#include <cstring>
#include <set>
#include <string>
#include <vector>

class Code;
struct Value;
class Compiler;
struct Function;
struct CallFrame;
struct Fiber;
struct Process;
struct String;
struct ProcessDef;
struct ClassDef;
class Interpreter;

typedef void (Compiler::*ParseFn)(bool canAssign);

enum Precedence
{
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_OR,          // ||
  PREC_AND,         // &&
  PREC_BITWISE_OR,  // |
  PREC_BITWISE_XOR, // ^
  PREC_BITWISE_AND, // &
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_SHIFT,       // << >>
  PREC_TERM,        // + -
  PREC_FACTOR,      // * / %
  PREC_UNARY,       // ! - ~ ++ --
  PREC_CALL,        // ()
  PREC_PRIMARY
};

struct ParseRule
{
  ParseFn prefix;
  ParseFn infix;
  Precedence prec;
};

#define MAX_IDENTIFIER_LENGTH 32
#define MAX_LOCALS 256

struct Local
{
  char name[MAX_IDENTIFIER_LENGTH];
  uint8 length;
  int depth;
  bool usedInitLocal;

  Local() : length(0), depth(-1), usedInitLocal(false) { name[0] = '\0'; }

  bool equals(const std::string &str) const
  {

    if (length != str.length())
    {
      return false;
    }

    return std::memcmp(name, str.c_str(), length) == 0;
  }

  bool equals(const char *str, size_t len) const
  {
    if (length != len)
    {
      return false;
    }
    return std::memcmp(name, str, length) == 0;
  }
};

#define MAX_LOOP_DEPTH 32
#define MAX_BREAKS_PER_LOOP 256

struct LoopContext
{
  int loopStart;
  int breakJumps[MAX_BREAKS_PER_LOOP];
  int breakCount;
  int scopeDepth;
  bool isForeach;

  LoopContext() : loopStart(0), breakCount(0), scopeDepth(0) {}

  bool addBreak(int jump)
  {
    if (breakCount >= MAX_BREAKS_PER_LOOP)
    {

      return false;
    }
    breakJumps[breakCount++] = jump;
    return true;
  }
};

struct Label
{
  std::string name;
  int offset;
};

struct GotoJump
{
  std::string target;
  int jumpOffset;
};

#define MAX_LOCALS 256
class Compiler
{
public:
  Compiler(Interpreter *vm);
  ~Compiler();

  void setFileLoader(FileLoaderCallback loader, void *userdata = nullptr);

  ProcessDef *compile(const std::string &source);
  ProcessDef *compileExpression(const std::string &source);

  void clear();

private:
  Interpreter *vm_;
  Lexer *lexer;
  Token current;
  Token previous;
  Token next;

  int cursor;

  FunctionType currentFunctionType;
  Function *function;
  Code *currentChunk;
 
  ClassDef *currentClass;
  ProcessDef *currentProcess;
  Vector<String *> argNames;
  std::vector<Token> tokens;

  bool hadError;
  bool panicMode;
  int tryDepth;  
  int scopeDepth;
  Local locals_[MAX_LOCALS];
  int localCount_;

  LoopContext loopContexts_[MAX_LOOP_DEPTH];
  int loopDepth_;
  bool isProcess_;

  std::vector<Label> labels;
  std::vector<GotoJump> pendingGotos;
  std::vector<GotoJump> pendingGosubs;

  // Token management
  void advance();
  Token peek(int offset = 0);

  bool checkNext(TokenType t);

  bool check(TokenType type);
  bool match(TokenType type);
  void consume(TokenType type, const char *message);

  void beginLoop(int loopStart,bool isForeach = false);
  void endLoop();
  void emitBreak();
  void pushScope();
  void popScope();
  void emitContinue();
  int discardLocals(int depth);
  void breakStatement();
  void continueStatement();

  // Error handling
  void error(const char *message);
  void errorAt(Token &token, const char *message);
  void errorAtCurrent(const char *message);
  void fail(const char *format, ...);
  void synchronize();

  // Bytecode emission
  void emitByte(uint8 byte);
  void emitBytes(uint8 byte1, uint8 byte2);
  void emitDiscard(uint8 count);
  void emitReturn();
  void emitConstant(Value value);
  uint8 makeConstant(Value value);

  int emitJump(uint8 instruction);
  void patchJump(int offset);

  void emitLoop(int loopStart);

  // Pratt parser
  void expression();
  void parsePrecedence(Precedence precedence);
  ParseRule *getRule(TokenType type);

  // Parse functions (prefix)
  void number(bool canAssign);
  void string(bool canAssign);
  void literal(bool canAssign);
  void grouping(bool canAssign);
  void unary(bool canAssign);
  void variable(bool canAssign);
  void lengthExpression(bool canAssign);
  void mathUnary(bool canAssign);
  void mathBinary(bool canAssign);
  void expressionClock(bool canAssign);

  // Parse functions (infix)
  void binary(bool canAssign);
  void and_(bool canAssign);
  void or_(bool canAssign);
  void call(bool canAssign);

  // Statements
  void declaration();
  void statement();
  void varDeclaration();
  void funDeclaration();
  void processDeclaration();
  void expressionStatement();
  void printStatement();
  void ifStatement();
  void whileStatement();
  void doWhileStatement();
  void loopStatement();
  void switchStatement();
  void forStatement();
  void foreachStatement();
  void returnStatement();
  void block();
  void yieldStatement();
  void fiberStatement();

  void tryStatement();
  void throwStatement();

  void dot(bool canAssign);
  void self(bool canAssign);
  void super(bool canAssign);

  void labelStatement();
  void gotoStatement();
  void gosubStatement();
  void resolveGotos();
  void resolveGosubs();
  void emitGosubTo(int targetOffset);
  void patchJumpTo(int operandOffset, int targetOffset);

  void handle_assignment(uint8 getOp, uint8 setOp, int arg, bool canAssign);

  void prefixIncrement(bool canAssign);
  void prefixDecrement(bool canAssign);

  // Variables
  uint8 identifierConstant(Token &name);
  void namedVariable(Token &name, bool canAssign);
  void defineVariable(uint8 global);
  void declareVariable();
  void addLocal(Token &name);
  int resolveLocal(Token &name);
  void markInitialized();

  uint8 argumentList();

  void compileFunction(Function *func, bool isProcess);
  void compileProcess(const std::string &name);

  bool isProcessFunction(const char *name) const;

  void structDeclaration();
  void arrayLiteral(bool canAssign);
  void subscript(bool canAssign);
  void mapLiteral(bool canAssign);
  void bufferLiteral(bool canAssign);

  void classDeclaration();
  void method(ClassDef *classDef);

  // Scope
  void beginScope();
  void endScope();

  bool inProcessFunction() const;

  void initRules();
  

  void frameStatement();
  void exitStatement();

  void includeStatement();
  void parseImport();
  void parseUsing();

  FileLoaderCallback fileLoader = nullptr;
  void *fileLoaderUserdata = nullptr;
  std::set<std::string> includedFiles;
      std::set<std::string> importedModules;
    std::set<std::string> usingModules;

  // HashSet<String *, StringHasher, StringEq> importedModules;
  // HashSet<String *, StringHasher, StringEq> usingModules;

  static ParseRule rules[TOKEN_COUNT];
};