
#include "pch.h"
#include "Vm.hpp"



extern void printValue(const Value &v);
extern void debugValue(const Value &v);
extern void printValueln(const Value &v);



Scope::Scope()
{
    name = "local";
    parent = nullptr;
    level = 0;
}

Scope::Scope(const char *name, Scope *parent, int level)
{
    this->name = name;
    this->name += "_";
    this->name += String(level);
    //   String(name) + "_" + String(level);

    this->parent = parent;
    this->level = level;
    // INFO("New scope: %s %d", name, level);
}

Scope::~Scope()
{
    // INFO("Delete scope: %s", name.c_str());
}


bool Scope::define(const char *name, Value value)
{
    if (variables.contains(name))
    {
        return false;
    }
    variables.insert(name, std::move(value));
    return true;
}

bool Scope::add(const char *name, Value value)
{
    variables.insert(name, std::move(value));
    return true;
}

bool Scope::get(const char *name, Value &value) 
{
    if (variables.find(name, value))
    {
        return true;
    }
    return parent ? parent->get(name, value) : false;
}

bool Scope::contains(const char *name) const
{
    if (variables.contains(name))
    {
        return true;
    }
    return parent ? parent->contains(name) : false;
}

bool Scope::assign(const char *name, Value value)
{
    if (variables.change(name, std::move(value)))
    {
        return true;
    }
    return parent ? parent->assign(name, std::move(value)) : false;
}

void Scope::write(const char *name, Value value)
{
   variables.change(name, std::move(value));
}

bool Scope::read(const char *name, Value &value)
{
   return variables.find(name, value);
}


void Scope::print()
{
    if (parent != nullptr)
    {
        PRINT("Parent: %s", parent->name.c_str());
        parent->print();
    }
}

void Scope::clear()
{
    variables.clear();
}

Task *VirtualMachine::getCurrentTask()
{
    return currentTask;
}

Task *VirtualMachine::getTask(const char *name)
{
    Task *task;
    if (taskesMap.find(name, task))
    {
        return task;
    }
    return nullptr;
}

Task *VirtualMachine::newTask(const char *name)
{
    Task *task = new Task(this, name);
    currentTask = task;
    taskes.push_back(task);
    taskesMap.insert(name, task);
    return task;
}

Task *VirtualMachine::addTask(const char *name)
{
    Task *task = new Task(this, name);
    taskes.push_back(task);
    return task;
}

Process *VirtualMachine::AddProcess(const char *name)
{
    Process *task = new Process(this, name);
    return task;
}

void VirtualMachine::setMainTask()
{
    if (currentTask != mainTask)
    {
        // currentTask->disassembleCode(currentTask->name.c_str());
    }

    currentTask = mainTask;
}

void VirtualMachine::switchTask(Task *task)
{

    task->parent = currentTask;

    // INFO("switch task %s from %s",task->name.c_str(),task->parent->name.c_str());
    currentTask = task;
}

void VirtualMachine::Error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(2, format, args);
    va_end(args);
    panicMode = true;
}

void VirtualMachine::Warning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(1, format, args);
    va_end(args);
}

void VirtualMachine::Info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(0, format, args);
    va_end(args);
}

int VirtualMachine::callNativeFunction(const char *name, Value *args, u8 argCount)
{
    NativeFunctionObject *func;
    if (nativeFunctions.find(name, func))
    {
        if (func->arity != -1)
        {
            if (func->arity != argCount)
            {
                Error("Native function %s has wrong number of arguments. Expected %d but got %d", name, func->arity, argCount);
                return 0;
            }
        }
        // Info("Calling native function %s", name);
        return func->call(this, argCount, args);
    }

    Warning("Native function %s not found", name);
    return 0;
}

Task *VirtualMachine::getMainTask()
{
    return mainTask;
}

void VirtualMachine::disassemble()
{
    PRINT("Disassembly tasks");
}



void VirtualMachine::registerFunction(const char *name, NativeFunction func, size_t arity)
{
    if (nativeFunctions.contains(name))
    {
        Warning("Function %s already exists", name);
        return;
    }
    parser.addNative(name);
    nativeFunctions.insert(name, new NativeFunctionObject(func, name, arity));
}

bool VirtualMachine::registerVariable(const char *name, Value value)
{
    return global->add(name, std::move(value));
}

bool VirtualMachine::registerNumber(const char *name, double value)
{
    return global->add(name, NUMBER(value));
}

bool VirtualMachine::registerInteger(const char *name, int value)
{
    return global->add(name, INTEGER(value));
}

bool VirtualMachine::registerString(const char *name, const char *value)
{
    return global->add(name, STRING(value));
}


bool VirtualMachine::registerBoolean(const char *name, bool value)
{
    return global->add(name, BOOLEAN(value));
}


bool VirtualMachine::push(Value v)
{
    return currentTask->push(std::move(v));
}

Value VirtualMachine::pop()
{
    return currentTask->pop();
}

Value VirtualMachine::peek(int offset)
{
    return currentTask->peek(offset);
}

Value VirtualMachine::top()
{
    return currentTask->top();
}



VirtualMachine::VirtualMachine()
{
    hooks.instance_create_hook  = default_instance_create_hook;
    hooks.instance_destroy_hook = default_instance_destroy_hook;
    hooks.instance_pos_execute_hook = default_instance_pos_execute_hook;
    hooks.instance_pre_execute_hook = default_instance_pre_execute_hook;
    hooks.process_exec_hook = default_process_exec_hook;


    global = new Scope("global", nullptr, 0);
    mainTask = new Task(this, "__main__");
    currentTask = mainTask;
    run_process.push_back(mainTask);
    taskes.push_back(mainTask);
    taskesMap.insert("__main__", mainTask);

    scopeDepth = 0;
    panicMode = false;
    isHalt = false;
    isDone = false;
    parser.Init(this);
    // stackTop = stack;
}

bool VirtualMachine::Compile(String source)
{
    if (parser.Load(std::move(source)))
    {
        return parser.Process();
    }
    return false;
}

bool VirtualMachine::IsReady()
{
    return  !panicMode && !isHalt;
}

bool VirtualMachine::Update()
{
    if (panicMode || isHalt  || run_process.size() == 0)   return false;
    
    for (size_t i = 0; i < run_process.size(); i++)
    {
        Task *task = static_cast<Task *>(run_process[i]);
        
        //Process *process = static_cast<Process *>(run_process[i]);
        u8 state = task->Run();
        
        if (state==ABORTED || state == TERMINATED || state == FINISHED)
        {
            INFO("Task %s finished", task->name.c_str());
            task->PrintStack();
            run_process.remove(task);
            i--;
        }
        if (task->type == ObjectType::OPROCESS)
            task->update();
       // process->update();
    }

    return run_process.size() == 0;
}

VirtualMachine::~VirtualMachine()
{
    Arena::as().clear();
    Clear();
    delete global;
}

void VirtualMachine::Clear()
{

    nativeFunctions.clear();
    scriptFunctions.clear();
    taskes.clear();
    taskesMap.clear();
    run_process.clear();
    global->clear();
    mainTask = nullptr;
    currentTask = nullptr;
}


void VirtualMachine::beginScope()
{
    scopeDepth++;
}

void VirtualMachine::endScope()
{
    scopeDepth--;
}

int VirtualMachine::getScopeDepth()
{
    return scopeDepth;
}

bool VirtualMachine::isGlobalScope()
{
    return scopeDepth == 0;
}


long VirtualMachine::pop_int()
{
    Value value = pop();
    if (IS_NUMBER(value))
    {
        double number = AS_NUMBER(value);
        return (long)number;
    }
    Error("Expected number but got :");
    printValue(value);
    return 0;
}

double VirtualMachine::pop_double()
{
    Value value = pop();
    if (IS_NUMBER(value))
    {
        double number = AS_NUMBER(value);
        return number;
    }
    Error("Expected number but got :");
    printValue(value);
    return 0;
}

float VirtualMachine::pop_float()
{
    Value value = pop();
    if (IS_NUMBER(value))
    {
        double number = AS_NUMBER(value);
        return (float)number;
    }
    Error("Expected number but got :");
    printValue(value);
    return 0;
}

long VirtualMachine::pop_long()
{
    Value value = pop();
    if (IS_NUMBER(value))
    {
        double number = AS_NUMBER(value);
        return (long)number;
    }
    Error("Expected number but got :");
    printValue(value);
    return 0;
}

String VirtualMachine::pop_string()
{
    Value value = pop();
    if (IS_STRING(value))
    {
        String str = AS_STRING(value)->string;
        return str;
    }
    Error("Expected string but got :");
    printValue(value);
    return "";
}

bool VirtualMachine::pop_bool()
{
    Value value = pop();
    if (IS_BOOLEAN(value))
    {
        return AS_BOOLEAN(value);
    }
    Error("Expected bool but got :");
    printValue(value);
    return false;
}

bool VirtualMachine::pop_nil()
{
    Value value = pop();
    if (IS_NONE(value))
    {
        return true;
    }
    Error("Expected nil but got :");
    printValue(value);
    return false;
}


bool VirtualMachine::push_int(int value)
{
    double number = static_cast<double>(value);
    return push(std::move(NUMBER(number)));
}

bool VirtualMachine::push_double(double value)
{
    return push(std::move(NUMBER(value)));
}

bool VirtualMachine::push_bool(bool value)
{
    return push(std::move(BOOLEAN(value)));
}

bool VirtualMachine::push_nil()
{
    return push(std::move(NONE()));
}

bool VirtualMachine::push_string(const char *value)
{
    return push(std::move(STRING(value)));
}

bool VirtualMachine::push_string(const String &str)
{
    return push(std::move(STRING(str.c_str())));
}


bool VirtualMachine::Run()
{

   // if (isDone) return false;
   

   // mainTask->disassembleCode("main");
    bool result = RunTask();


    return result;
}

//***************************************************************************************************************** */
//***************************************************************************************************************** */
//***************************************************************************************************************** */

 static const size_t instructionsPerFrame = 60;
u8 VirtualMachine::RunTask()
{
    
    if (panicMode || isHalt) return FINISHED;
    if (isDone) return FINISHED;

     size_t instructionsExecuted = 0;

    //for (;;)
    while(instructionsExecuted < instructionsPerFrame)
    {
    

        u8 instruction = currentTask->read_byte();
        int line = currentTask->read_line();

        if (instruction >= COUNT)
        {
            Warning("Unknown instruction ");
            return ABORTED;
        }

    switch ((OpCode)instruction)
    {
        case OpCode::CONST:
        {

            Value value = currentTask->read_const();
            currentTask->push(std::move(value));

            break;
        }
        case OpCode::PUSH:
        {

            Value value = currentTask->read_const();
            currentTask->push(std::move(value));
            break;
        }
        case OpCode::POP:
        {
            currentTask->pop();
            break;
        }

        case OpCode::HALT:
        {
            isHalt = true;
            Warning("Halt!");
            return ABORTED;
        }
        case OpCode::PRINT:
        {
            Value value = currentTask->pop();
            // printValue(std::move(value));
            printValueln(std::move(value));
            break;
        }
        case OpCode::NOW:
        {
            currentTask->push(std::move(NUMBER(time_now())));
            break;
        }
        case OpCode::ADD:
        {
             Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {

                Value result = NUMBER(AS_NUMBER(a) + AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                String result = a.string->string + b.string->string;
                currentTask->push(std::move(STRING(result.c_str())));
            }
            else if (IS_STRING(a) && IS_NUMBER(b))
            {
                String number(AS_NUMBER(b));
                String result = a.string->string + number;
                currentTask->push(std::move(STRING(result.c_str())));
            }
            else if (IS_NUMBER(a) && IS_STRING(b))
            {
                String number(AS_NUMBER(a));
                String result = number + b.string->string;
                currentTask->push(std::move(STRING(result.c_str())));
            }
            else
            {

                Error("invalid 'adding' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::SUBTRACT:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = NUMBER(AS_NUMBER(a) - AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid  'subtract' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }

            break;
        }
        case OpCode::MULTIPLY:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = NUMBER(AS_NUMBER(a) * AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'multiply' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::DIVIDE:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                if (AS_NUMBER(b) == 0)
                {
                    Error("division by zero [line %d]", line);
                    panicMode = true;
                    return ABORTED;
                }
                Value result = NUMBER(AS_NUMBER(a) / AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'divide' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::MOD:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                double _a = AS_NUMBER(a);
                double divisor = AS_NUMBER(b);
                if (divisor == 0)
                {
                    Error("division by zero [line %d]", line);
                    panicMode = true;
                    return ABORTED;
                }
                double result = fmod(_a, divisor);
                if (result != 0 && ((_a < 0) != (divisor < 0)))
                {
                    result += divisor;
                }
                currentTask->push(std::move(NUMBER(result)));
            }
            else
            {
                Error("invalid 'mod' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::POWER:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = NUMBER(pow(AS_NUMBER(a), AS_NUMBER(b)));
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'power' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::NEGATE:
        {
            Value value = currentTask->pop();
            if (IS_NUMBER(value))
            {
                Value result = NUMBER(-AS_NUMBER(value));
                currentTask->push(std::move(result));
            }
            else if (IS_BOOLEAN(value))
            {
                Value result = BOOLEAN(!AS_BOOLEAN(value));
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'negate' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::NOT:
        {
            Value value = currentTask->pop();
            if (IS_BOOLEAN(value))
            {
                Value result = BOOLEAN(!AS_BOOLEAN(value));
                currentTask->push(std::move(result));
            }
            else if (IS_NUMBER(value))
            {
                Value result = BOOLEAN(AS_NUMBER(value) == 0);
                currentTask->push(std::move(result));
            }

            else
            {
                Error("invalid 'not' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::EQUAL:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            bool result = MatchValue(a, b);
            currentTask->push(BOOLEAN(result));

            break;
        }
        case OpCode::EVAL_EQUAL:
        {
            Value b = currentTask->peek(0);
            Value a = currentTask->peek(1);
            bool result = MatchValue(a, b);
            currentTask->push(BOOLEAN(result));
            break;
        }
        case OpCode::NOT_EQUAL:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) != AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string != b.string->string);
                currentTask->push(std::move(result));
            }
            else if (IS_BOOLEAN(a) && IS_BOOLEAN(b))
            {
                Value result = BOOLEAN(AS_BOOLEAN(a) != AS_BOOLEAN(b));
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'not equal' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::LESS:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) < AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() < b.string->string.length());
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'less' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }

            break;
        }
        case OpCode::LESS_EQUAL:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) <= AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() <= b.string->string.length());
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'less equal'   [line %d]", line);
                panicMode = true;
                return ABORTED;
            }

            break;
        }
        case OpCode::GREATER:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) > AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() > b.string->string.length());
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'greater'  [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::GREATER_EQUAL:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) >= AS_NUMBER(b));
                currentTask->push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() >= b.string->string.length());
                currentTask->push(std::move(result));
            }
            else
            {
                Error("invalid 'greater equal'    [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }

        case OpCode::XOR:
        {
            Value b = currentTask->pop();
            Value a = currentTask->pop();
            if (IS_BOOLEAN(a) && IS_BOOLEAN(b))
            {
                bool b_a = AS_BOOLEAN(a);
                bool b_b = AS_BOOLEAN(b);
                bool result = b_a ^ b_b;
                currentTask->push(std::move(BOOLEAN(result)));
            }
            else if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                int n_a = static_cast<int>(AS_NUMBER(a));
                int n_b = static_cast<int>(AS_NUMBER(b));
                int result = n_a ^ n_b;
                double number = static_cast<double>(result);
                currentTask->push(std::move(NUMBER(number)));
            }
            else if (IS_BOOLEAN(a) && IS_NUMBER(b))
            {
                bool b_a = AS_BOOLEAN(a);
                int n_b = static_cast<int>(AS_NUMBER(b));
                bool result = b_a ^ n_b;
                currentTask->push(std::move(BOOLEAN(result)));
            }
            else if (IS_NUMBER(a) && IS_BOOLEAN(b))
            {
                int n_a = static_cast<int>(AS_NUMBER(a));
                bool b_b = AS_BOOLEAN(b);
                bool result = n_a ^ b_b;
                currentTask->push(std::move(BOOLEAN(result)));
            }
            else
            {
                Error("invalid 'xor' operands [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            break;
        }
        case OpCode::VARIAVEL_DEFINE:
        {

            Value constant = currentTask->read_const();
            if (!IS_STRING(constant))
            {
                Error("Variable  name must be string [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            const char *name = AS_RAW_STRING(constant);
            Value value = currentTask->pop();

            if (getScopeDepth() == 0)
            {
                if (!global->define(name, std::move(value)))
                {
                    Warning("Already a global variable with '%s' name.", name);
                    break;
                }
            }
            else
            {
                //   PRINT("Added local variable %s\n", name);
                if (!currentTask->local.define(name, std::move(value)))
                {
                    Warning("Already a variable with '%s' name.", name);
                    break;
                }
            }
            //  printValue(value);

            break;
        }

        case OpCode::VARIAVEL_ASSIGN:
        {
            Value constant = currentTask->read_const();
            if (!IS_STRING(constant))
            {
                Error("Variable  name must be string [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            const char *name = AS_RAW_STRING(constant);
            Value value = currentTask->peek();

            if (!currentTask->local.assign(name, value))
            {
                if (!global->assign(name, value))
                {
                    Warning("Undefined variable '%s' [line %d]", name, line);
                }
            }

            break;
        }
        case OpCode::VARIAVEL_GET:
        {
            Value constant = currentTask->read_const();
            if (!IS_STRING(constant))
            {
                Error("Variable  names must be string [line %d]", line);
                panicMode = true;
                return ABORTED;
            }
            const char *name = AS_RAW_STRING(constant);
            Value value;

            if (currentTask->local.get(name, value))
            {

                currentTask->push(value);
                break;
            }
            else
            {
                if (global->get(name, value))
                {
                    currentTask->push(value);
                }
                else
                {
                    Warning("[READ] Undefined variable '%s' [line %d] %d", name, line, getScopeDepth());

                    currentTask->push(std::move(NONE()));
                }
            }

            break;
        }

        case OpCode::JUMP_IF_FALSE:
        {
            u16 offset = currentTask->read_short();
            Value condition = currentTask->pop();

            if (isFalsey(condition))
            {
                currentTask->ip += offset;
            }
            break;
        }
        case OpCode::JUMP_IF_TRUE:
        {
            u16 offset = currentTask->read_short();
            Value condition = currentTask->pop();
            if (!isFalsey(condition))
            {
                currentTask->ip -= offset;
            }
            break;
        }
        case OpCode::DROP:
        {

            break;
        }
        case OpCode::JUMP:
        {
            u16 offset = currentTask->read_short();
            currentTask->ip += offset;
            break;
        }
        case OpCode::DUP:
        {
            Value value = currentTask->peek(0);
            currentTask->push(value);
            break;
        }
        case OpCode::JUMP_BACK:
        {
            uint16_t offset = currentTask->read_short();
            currentTask->ip -= offset;
            break;
        }

        case OpCode::ENTER_SCOPE:
        {

            
            beginScope();

            // if (currentTask->type == ObjectType::OPROCESS)
            // {
            //     currentTask->create();
            // }
           

            break;
        }
        case OpCode::EXIT_SCOPE:
        {

            endScope();
            currentTask->exitScope();
           

            break;
        }
        case OpCode::CALL:
        {
           // currentTask->PrintStack();
            size_t argCount = (size_t)currentTask->read_byte();
            Value func = currentTask->peek(argCount);
            Value args[256];
            size_t index = argCount - 1;
            for (size_t i = 0; i < argCount; i++)
            {
                args[index] = std::move(currentTask->peek(argCount - i - 1));
                index--;
            }
           
          // PRINT("Calling %s with %d arguments\n", AS_RAW_STRING(func), argCount);
            Value result;
            int count = callNativeFunction(AS_RAW_STRING(func), args, argCount);
            if (count >= 1)
            {
                 result =  currentTask->pop();
            }
            
            currentTask->pop(argCount + 1);

             if (count >= 1)
            {
                currentTask->push(std::move(result));
            }
          // currentTask->PrintStack();
      
            break;
        }
        case OpCode::CALL_SCRIPT:
        {

            int argCount = (int)currentTask->read_byte();
            Value func = currentTask->peek(argCount);

            Task *callTask = getTask(AS_RAW_STRING(func));
            if (!callTask)
            {
                currentTask->pop(argCount);
                Error("Task '%s' not defined [line %d]", AS_RAW_STRING(func), line);
                panicMode = true;
                return ABORTED;
            }
            if (callTask->argsCount != argCount)
            {
                Error("Function %s, Expected %ld arguments but got %ld [line %d]", AS_RAW_STRING(func), callTask->argsCount, argCount, line);
                panicMode = true;
                return ABORTED;
            }
            Task *process = addTask(AS_RAW_STRING(func));
            for (int i = argCount - 1; i >= 0; i--)
            {
                Value arg = currentTask->pop();
                const char *name = callTask->args[i].c_str();
                process->local.add(name, std::move(arg));
            }

         //  Arena::as().add(process);
            run_process.push_back(process);
            process->code = callTask->code;
            process->lines = callTask->lines;
            process->constants = callTask->constants;
            process->reset();
            //   process->disassembleCode(process->name.c_str());
            currentTask->pop(); // drop the function name
            process->parent = currentTask;
           // currentTask = process;

            break;
        }
        case OpCode::RETURN:
        {

            if (matchString("__main__", currentTask->name.c_str(),currentTask->name.length()))
            {
                INFO("RETURN %s done end execution", currentTask->name.c_str());
                isDone = true;
                return TERMINATED;
            }

            //  PrintStack();
            Value result = currentTask->pop();
            if (currentTask->parent != nullptr)
            {
                Task *parent = currentTask->parent;
                
                 currentTask->unmark();
             

                 INFO("RETURN %s parent %s", currentTask->name.c_str(),parent->name.c_str());
                currentTask->parent = nullptr;
                currentTask = parent;
            }

            currentTask->push(std::move(result));

            break;
        }
        case OpCode::CALL_PROCESS:
        {

            int argCount = (int)currentTask->read_byte();
            Value func = currentTask->peek(argCount);
            Task *callTask = getTask(AS_RAW_STRING(func));
            if (!callTask)
            {
                currentTask->pop(argCount);
                Error("Process '%s' not defined [line %d]", AS_RAW_STRING(func), line);
                panicMode = true;
                return ABORTED;
            }
            if (callTask->argsCount != argCount)
            {
                Error("Process %s, Expected %ld arguments but got %ld [line %d]", AS_RAW_STRING(func), callTask->argsCount, argCount, line);
                panicMode = true;
                return ABORTED;
            }
            Task *process = AddProcess(AS_RAW_STRING(func));
            for (int i = argCount - 1; i >= 0; i--)
            {
                Value arg = currentTask->pop();
                const char *name = callTask->args[i].c_str();
                process->local.add(name, std::move(arg));
            }

            run_process.push_back(process);
            process->code = callTask->code;
            process->lines = callTask->lines;
            process->constants = callTask->constants;
            process->reset();
            process->parent = currentTask;
            //currentTask = process;


            break;
        }
        case OpCode::RETURN_PROCESS:
        {
            if (currentTask->parent != nullptr)
            {
                Task *parent = currentTask->parent;

                if (currentTask->type == ObjectType::OPROCESS)
                {
                    currentTask->remove();
                }
                 
                 INFO("RETURN %s parent %s", currentTask->name.c_str(),parent->name.c_str());
               // currentTask->parent = nullptr;
              //  currentTask = parent;
            }

            break;
        }
        case OpCode::RETURN_NATIVE:
        {
            //INFO("RETURN %s done", currentTask->name.c_str());
            //currentTask->PrintStack();
            //currentTask->pop();
            break;
        }
        case OpCode::FRAME:
        {
            INFO("FRAME %s", currentTask->name.c_str());
            

            return RUNNING;
        }

        case OpCode::NIL:
        {
            currentTask->push(std::move(NONE()));
            break;
        }


        default:
        {
            Error(" %s running %d with unknown '%d' opcode ", currentTask->name.c_str(), currentTask->ip, (int)instruction);
            panicMode = true;
            return ABORTED;
        }
        }
       
        instructionsExecuted++;

        if (instructionsExecuted >= instructionsPerFrame)
        {
            return RUNNING;
        }
    

    }

    return RUNNING;
}
