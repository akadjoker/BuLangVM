#pragma once

#include "Config.hpp"
#include "Trivial.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Arena.hpp"
#include "Value.hpp"

class Interpreter;
class Lexer;
class Compiler;
class Parser;
class Visitor;
class ByteGenerator;


enum OpCode
{
    ZERO = 0,
    PUSH = 1,
    POP = 2,
    CONST = 3,
    RETURN  = 4,
    HALT = 5,
    PRINT = 6,
    NOW,
    FRAME,
    PROGRAM,

    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,

    MOD,
    NEGATE,

    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    TRUE,
    FALSE,
    NOT,
    AND,
    OR,
    XOR,
    INC,
    DEC,
    RINC,
    RDEC,
    SHL,
    SHR,

    OPMUL,
    OPADD,
    OPSUB,
    OPDIV,
    

    GLOBAL_DEFINE,
    GLOBAL_SET,
    GLOBAL_GET,
    GLOBAL_ASSIGN,

    
    ENTER_SCOPE,
    EXIT_SCOPE,



    SWITCH,
    CASE,
    SWITCH_DEFAULT,

    DUP,
    EVAL_EQUAL,



    
    BREAK,
    CONTINUE,


    SAVE,
    RESTORE,


    
    NIL,

    
    LOOP,
    BACK,
    JUMP,
    JUMP_IF_FALSE,
    JUMP_IF_TRUE,


    CALL_NATIVE,
    CALL_SCRIPT,

    COUNT,
};



#define MAX_FRAMES 64
#define STACK_MAX (MAX_FRAMES * UINT8_MAX)

#define INTERPRET_RUNTIME_ERROR (1)
#define INTERPRET_HALT (2)
#define INTERPRET_OK (0)

struct Frame
{
    Compiler    *compiler;
    u8          *ip;
    Value       *slots;
    Scope       *scope;

};


struct JumpLabel
{
    String name;
    int    position;
};


struct PatchEntry 
{
    String name;  
    int address;       // Endereço no bytecode onde o `goto` foi emitido
};

class Compiler 
{
 
    public:

    Compiler(const Chars &name,Interpreter *i, Compiler *parent = nullptr);
    ~Compiler();


    u8 Run();
    void Disassemble();
    

    bool push( Value v);
    Value pop();
    Value peek(int offset = 0);
    Value top();
    void pop(u32 count);
    void PrintStack();

    Scope *local;

    void createScope(Scope *parent = nullptr);

    void abort();

      u8 addConst(Value v);
    u8 addConstString(const char *str);
    u8 addConstNumber(double number);
    u8 addConstBoolean(bool b);

    size_t argsCount() const { return args.size(); }
    bool addArg(const Chars &name);
  

private:
    friend class Interpreter;
    friend class Parser;
    friend class ByteGenerator;
    Interpreter *vm;
    Compiler *parent;
    Chars name;
    Chunk *chunk;

    Vector<Scope*> scopeStack;
    Trivial<int> doContinue;
    int loopStart = -1;

  
    int breakCount = 0;
    int breaks[UINT8_MAX]{-1};
    
    
    Value stack[STACK_MAX];
    Value* stackTop;
    Frame frames[MAX_FRAMES];
    Vector<Chars> args;
    int frameCount;

    Map<String, int> labels;
    Vector<JumpLabel> jumpLabels;

    Vector<PatchEntry> patchList;

    bool  addLabel(const String &name,int position);
    bool  getLabel(const String &name, JumpLabel **label);
    JumpLabel *getLabel( int index);
    bool  labelExists(const String &name,int *index) const;
    int totalLabels() const { return (int)jumpLabels.size(); }    


    void write_byte(u8 byte, int line);


    void set_frame();


    Vector<Value> constants;

    u8 makeConstant( Value value);
    void writeByte(u8 byte, int line);
    void writeBytes(u8 byte1, u8 byte2,int line);
    void writeConstant( Value value,int line);


  
    
    int breakJumpCount =0;
    int breakJumps[UINT8_MAX]{-1};


    void disassembleCode(const char *name);
    u32 disassembleInstruction(u32 offset);
    u32 constantInstruction(const char *name, u32 offset);
    u32 simpleInstruction(const char *name, u32 offset);
    u32 byteInstruction(const char *name, u32 offset);
    u32 jumpInstruction(const char *name, u32 sign, u32 offset);


};

class Interpreter
{
private:
    bool panicMode{ false };
    ByteGenerator* generator{ nullptr };
    Compiler* compiler{ nullptr };

     


public:
    Interpreter();
    ~Interpreter();

    bool Load(const String& source);
    
    void Disassemble();

    u8 Run();
    
    void Clear();
    
protected:
    friend class Lexer;
    friend class Parser;
    friend class Compiler;
    friend class ByteGenerator;
    Compiler                *current;
    Vector<Compiler*>       compilers;
    Map<Chars, Compiler*>  compilerMap;

    Compiler *newCompiler(const Chars &name, Compiler *parent=nullptr);
    Compiler *getCompiler(const Chars &name);

    Compiler *getCurrent() { return current; }



    void Error(const String& message);
    void Warning(const String& message);
    void Info(const String& message);



    void Error(const char* format, ...);
    void Warning(const char* format, ...);
    void Info(const char* format, ...);
};

