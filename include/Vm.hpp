#pragma once
#include "Types.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
class VirtualMachine;


enum OpCode
{
    NONE = 0,
    PUSH,
    POP,
    CONST,
    RETURN,
    HALT,
    PRINT,
    NOW,
    FRAME,

    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,

    MOD,
    POWER,
    NEGATE,

    EQUAL,
    NOT_EQUAL,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,

    NOT,
    XOR,
    INC,
    DEC,
    SHL,
    SHR,

    ENTER_SCOPE,
    EXIT_SCOPE,


    VARIAVEL_DEFINE,
    VARIAVEL_GET,
    VARIAVEL_ASSIGN,



    SWITCH,
    CASE,
    SWITCH_DEFAULT,

    DUP,
    EVAL_EQUAL,

    JUMP_BACK,

    LOOP_BEGIN,
    LOOP_END,

    BREAK,
    CONTINUE,
    
    DROP,
    CALL,
    CALL_SCRIPT,
    CALL_PROCESS,
    RETURN_DEF,
    RETURN_PROCESS,
    RETURN_NATIVE,
    
    NIL,

    JUMP,
    JUMP_IF_FALSE,
    JUMP_IF_TRUE,
    COUNT,
};

class Task;



#define FRAMES_MAX 64
#define STACK_MAX 1024


static const int RUNNING = 0;
static const int FINISHED = 1;
static const int ABORTED = 2;
static const int TERMINATED = 3;

struct Scope 
{
    String name;
    Scope *parent;
    int level;
    HashTable<Value> variables;
    Scope();
    Scope(const char *name, Scope *parent, int level);
    virtual ~Scope();
    bool define(const char *name, Value value);
    bool add(const char *name, Value value);
    bool get(const char *name, Value &value);
    bool contains(const char* name) const;
    bool assign(const char* name, Value value);
    
    void  write(const char *name, Value value);
    bool  read(const char *name, Value &value);

    void print();
    void clear();
};

class Task : public Traceable
{
private:
    friend class VirtualMachine;
    friend class Parser;

 

    Vector<u8> code;
    Vector<u8> breakJumps;
    Vector<Value> constants;
    Vector<String> args;


    Vector<int> lines;
    

    


    
    s32 innermostLoopStart = {-1};
    u32 innermostLoopScopeDepth = {0};
    s32 loopStart{-1};
    s32 exitJump{-1};
    s32 currentContinueJump{-1};
    s32 currentBreakJump{-1};
    u32 frameDepth;
    



    Value stack[STACK_MAX];
    Value* stackTop;
    
    
    u8  *ip;
    u8 *lastIp;
    int *lineBuffer;
    int *lastLine;

    u8 argsCount;

    void exitScope();




    bool  push(Value v);
    Value pop();
    Value peek(int offset = 0);
    void  pop(u32 count);
    Value top();

    int read_line();


    u8  current_byte();
    u8  current_next_byte();
    u16 current_short();
    Value current_const();


    u8 read_byte();
    u16 read_short();

    u16 read_short(u8 instruction);
    Value read_const_at(u8 index);

    Value read_const();
    Value read_const(int index);
    u8 makeConstant( Value value);
    void writeByte(u8 byte, int line);
    void writeBytes(u8 byte1, u8 byte2,int line);
    void writeConstant(const Value &value,int line);
    
    void writeConstantVar(const char *name, Value value, int line);

    void disassembleCode(const char *name);
    u32 disassembleInstruction(u32 offset);
    u32 constantInstruction(const char *name, u32 offset);
    u32 simpleInstruction(const char *name, u32 offset);
    u32 byteInstruction(const char *name, u32 offset);
    u32 jumpInstruction(const char *name, u32 sign, u32 offset);
    u32 varInstruction(const char *name, u32 offset);
    void PrintStack();



    void patch(u32 offset);
    void addToInPlaceJumpOffsetList(int offset, int jumpAddress);

    void go_to(int offset, u8 jumpAddress);
    void patchBreaks(int breakJump);

    u32 code_size();

protected:
    String name;
    u64 ID;
    Scope local;
    bool m_done;
    Task *parent;
    VirtualMachine *vm;

public:
    Task(VirtualMachine *vm, const char *name);
    virtual ~Task();

    void reset();


    void write_byte(u8 byte, int line);

    bool addArgs(const String &name);

    u8 addConst(Value v);
    u8 addConstString(const char *str);
    u8 addConstNumber(double number);

    virtual void create(){};
    virtual void remove(){};
    virtual void update(){};

    u8 Run();

    bool IsDone();
    void Done();

};

const int IX = 0;
const int IY = 1;
const int IGRAPH = 2;


struct Instance 
{
    u32 ID;
    s32 locals[32];
    String name;
    Instance *father;
    Instance *son;
    Instance *smallBrother;
    Instance *bigBrother;
    void *user_data;
    Instance()
    {
       
        name = "";
        father = nullptr;
        son = nullptr;
        smallBrother = nullptr;
        bigBrother = nullptr;
    }
};

class Process : public Task 
{

protected:
    Value x;
    Value y;
    Value graph;
    bool isCreated;
    u32 frames;

public:
    Process(VirtualMachine *vm, const char *name);
    ~Process();
    void create() override;
    void update() override;
    void remove() override;
    Instance instance;
};

struct Hook
{
    void (* instance_create_hook)(Instance *);
    void (* instance_destroy_hook)(Instance *);
    void (* instance_pre_execute_hook)(Instance *);
    void (* instance_pos_execute_hook)(Instance *);
    void (* process_exec_hook)(Instance *);
};

void default_instance_create_hook(Instance *instance);
void default_instance_destroy_hook(Instance *instance);
void default_instance_pre_execute_hook(Instance *instance);
void default_instance_pos_execute_hook(Instance *instance);
void default_process_exec_hook(Instance *instance);

class VirtualMachine
{
    friend class Process;
    friend class Task;
    friend class Lexer;
    friend class Parser;

    Parser parser;

    Vector<Task *> taskes;
    HashTable<Task *> taskesMap;
    HashTable<NativeFunctionObject *> nativeFunctions;
    HashTable<Value> scriptFunctions;
    Scope *global;



    Task *mainTask;
    Task *currentTask;
    
    int scopeDepth;
   
   

    bool panicMode;
    bool isHalt;
    bool isDone;

    Value addFunction(const char *name, int arity);

    Task *newTask(const char *name);
    Task *getCurrentTask();
    Task *getTask(const char *name);

    Task *addTask(const char *name);
    Process *AddProcess(const char *name);

    void setMainTask();
    void switchTask(Task *task);

    void Error(const char *format, ...);
    void Warning(const char *format, ...);
    void Info(const char *format, ...);

    int callNativeFunction(const char *name, Value *args, u8 argCount);

    u8 RunTask();


    void beginScope();
    void endScope();

    int  getScopeDepth();
    bool isGlobalScope();

    List run_process;

    void op_add(Task *task,int line);
    void op_sub(Task *task,int line);
    void op_mul(Task *task,int line);
    void op_div(Task *task,int line);
    void op_mod(Task *task,int line);
    void op_neg(Task *task,int line);
    void op_not(Task *task,int line);
    void op_eq(Task *task,int line);
    void op_neq(Task *task,int line);
    void op_gte(Task *task,int line);
    void op_lte(Task *task,int line);
    void op_gt(Task *task,int line);
    void op_lt(Task *task,int line);
    void op_and(Task *task,int line);
    void op_or(Task *task,int line);
    

public:
    VirtualMachine();
    ~VirtualMachine();
    
    void Clear();
    
    bool Run();
    bool Compile(String source);
    bool IsReady();
    bool Update();


    Task *getMainTask();
    void disassemble();

    void registerFunction(const char *name, NativeFunction func, size_t arity);
    bool registerVariable(const char *name, Value value);
    bool registerNumber(const char *name, double value);
    bool registerInteger(const char *name, int value);
    bool registerString(const char *name, const char *value);
    bool registerBoolean(const char *name, bool value);
    bool registerNil(const char *name);

    bool push(Value v);
    Value pop();
    Value peek(int offset = 0);
    Value top();

    bool push_int(int value);
    bool push_double(double value);
    bool push_float(float value);
    bool push_long(long value);
    bool push_string(const char *str);
    bool push_string(const String &str);
    bool push_bool(bool value);
    bool push_nil();

    bool is_number();
    bool is_string();
    bool is_bool();
    bool is_nil();

    long   pop_int();
    double pop_double();
    float  pop_float();
    long   pop_long();
    String pop_string();
    bool   pop_bool();
    bool   pop_nil();

    Hook hooks;
};


