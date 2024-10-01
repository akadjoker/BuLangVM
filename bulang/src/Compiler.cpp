#include "pch.h"
#define IMPLEMENTATION
#include "Utils.hpp"
#include "Interpreter.hpp"





void debugValue(const Value &v)
{
    switch (v.type)
    {
    case ValueType::VUNDEFINED:
        printf("undefined");
        break;
    case ValueType::VSTRING:
        if (v.string->string == "\n")
            ;
       // printf("\\n");
        else 
            printf("%s", v.string->string.c_str());
        break;
    case ValueType::VNUMBER:
        printf("%g", v.number);
        break;
    case ValueType::VBOOLEAN:
        printf("%s", v.boolean ? "true" : "false");
        break;
    case ValueType::VNONE:
        printf("nil");
        break;
   
    default:
        printf("Unknow value %d",(int) v.type);
        break;
    }
}
void printValueln(const Value &v)
{
    switch (v.type)
    {
    case ValueType::VUNDEFINED:
        printf("undefined\n");
        break;
    case ValueType::VSTRING:
        printf("%s\n", v.string->string.c_str());
        break;
    case ValueType::VNUMBER:
        printf("%g\n", v.number);
        break;
    case ValueType::VBOOLEAN:
        printf("%s\n", v.boolean ? "true" : "false");
        break;
    case ValueType::VNONE:
        printf("nil\n");
        break;
  
    default:
        printf("Unknow value ");
        break;
    }
}

void printValue(const Value &v)
{
    switch (v.type)
    {
    case ValueType::VUNDEFINED:
        PRINT("undefined");
        break;
    case ValueType::VSTRING:
        PRINT("%s", v.string->string.c_str());
        break;
    case ValueType::VNUMBER:
        PRINT("%g", v.number);
        break;
    case ValueType::VBOOLEAN:
        PRINT("%s", v.boolean ? "true" : "false");
        break;
    case ValueType::VNONE:
        PRINT("nil");
        break;

    default:
        PRINT("Unknow value ");
        break;
    }
}


Compiler::Compiler(const Chars &name,Interpreter *i, Compiler *parent)
{
    this->name = name;
    stackTop = stack;
    chunk = nullptr;
    parent = parent;
    vm = i;
    doContinue.reserve(256);
    chunk = new Chunk();
    scopeStack.reserve(1024);
    if (name=="__main__")
    {
        createScope();
        scopeStack.push_back(local);
    }
  
}

Compiler::~Compiler()
{
    INFO("Destroying compiler %s", name.c_str());
    delete chunk;
 //   Factory::as().DestroyScope(local);

}

void Compiler::set_frame()
{
    frameCount = 0;
    Frame *frame = &frames[frameCount++];
    frame->compiler = this;
    frame->slots = stack;
    frame->ip = chunk->code;
}

Value Compiler::peek(int offset)
{
     return stackTop[-1 - offset];
}

bool Compiler::push( Value v)
{
    if (stackTop - stack >= STACK_MAX)
        return false;
    *stackTop =std::move(v);
    stackTop++;
    return true;
}

Value Compiler::pop()
{
    stackTop--;
    return *stackTop;
}

void Compiler::pop(u32 count)
{
    for (u32 i = 0; i < count; i++)
    {
        pop();
    }
}

void Compiler::PrintStack()
{
   // printf("          ");
    for (Value *slot = stack; slot < stackTop; slot++)
    {
        printf("[ ");
        debugValue(*slot);
        printf("] ");
    }

 ///   printf("\n");
}

void Compiler::createScope(Scope *parent )
{
    local = Factory::as().CreateScope(parent);
}

void Compiler::abort()
{
    write_byte(OpCode::HALT, 0);
}

u8 Compiler::addConst(Value v)
{

    // for (size_t i = 0; i < constants.size(); i++)
    // {
    //     if (MatchValue(constants[i], v))
    //     {
    //         INFO("Found constant %d", i);
    //         return (u8)i;
    //     }
    // }
 
    constants.push_back(std::move(v));
    if (constants.size() > 256)
    {
        vm->Error("Too many constants in  compiler");
        return 0;
    }
  
    return (u8)  constants.size() - 1;
}

u8 Compiler::addConstString(const char *str)
{
    return addConst(to_string(str));
}

u8 Compiler::addConstNumber(double number)
{
    return addConst(to_number(number));
}

u8 Compiler::addConstBoolean(bool b)
{
    return addConst(to_boolean(b));
}

bool Compiler::addArg(const Chars &name)
{
    if (argsCount() >= 255)
    {
        vm->Error("Too many arguments in  compiler");
        return false;
    }
    for (size_t i = 0; i < args.size(); i++)
    {
        if (args[i] == name)
        {
            return false;
        }
    }
    args.push_back(name);
    return true;
}

bool Compiler::addLabel(const String &name,int position)
{
    int index = (int)jumpLabels.size();
    if (labels.insert(name, index))
    {
        INFO("Added label %s at %d", name.c_str(), position);
        jumpLabels.push_back({ name, position});
        return true;
    }
    return false;

}

bool Compiler::labelExists(const String &name,int *index) const
{
    return labels.find(name, *index);
}




bool Compiler::getLabel(const String &name,JumpLabel** label) 
{
    int index = -1;
    if (labels.find(name, index))
    {
        *label = &jumpLabels[index];
        return true;
    }
 
    return false;
}

JumpLabel *Compiler::getLabel( int index)
{
    if (index < 0 || index >= (int)jumpLabels.size())
    {
        vm->Error("Label index out of bounds.");
        return nullptr;
    }
    return &jumpLabels[index];
}

void Compiler::write_byte(u8 byte, int line)
{
    chunk->write(byte, line);
}

void Compiler::writeByte(u8 byte, int line)
{
    write_byte(byte, line);
}
// 

void Compiler::writeBytes(u8 byte1, u8 byte2, int line)
{
    writeByte(byte1, line);
    writeByte(byte2, line);
}
void Compiler::writeConstant(Value value, int line)
{
    writeBytes(OpCode::CONST, makeConstant(std::move(value)), line);
}
u8 Compiler::makeConstant(Value value)
{
    int constant = (int)addConst(std::move(value));
    if (constant > 255)
    {
        vm->Error("Too many constants in  task");
        return 0;
    }
    return (u8)constant;
}




const String OpCodeNames[] = 
{
    "ZERO",
    "PUSH",
    "POP",
    "CONST",
    "RETURN",
    "HALT",
    "PRINT",
    "NOW",
    "FRAME",
    "PROGRAM",

    "ADD",
    "SUBTRACT",
    "MULTIPLY",
    "DIVIDE",

    "MOD",
    "NEGATE",

    "EQUAL",
    "NOT_EQUAL",
    "GREATER",
    "GREATER_EQUAL",
    "LESS",
  
    "LESS_EQUAL",

    "TRUE",
    "FALSE",
    "NOT",
    "AND",
    "OR",
    "XOR",
    "INC",
    "DEC",
    "RINC",
    "RDEC",

    "SHL",
    "SHR",

    "OPMUL",
    "OPADD",
    "OPSUB",
    "OPDIV",


    "GLOBAL_DEFINE",
    "GLOBAL_DEFINE_SET",
    "GLOBAL_GET",
    "GLOBAL_ASSIGN",

    "ENTER_SCOPE",
    "EXIT_SCOPE",

    "SWITCH",
    "CASE",
    "SWITCH_DEFAULT",

    "DUP",
    "EVAL_EQUAL",

    

    "LOOP",
    
    "BREAK",
    "CONTINUE",

    "LABEL",
    "GOTO",
    "GOSUB",

    "NIL",

    "JUMP",
    "JUMP_IF_FALSE",
    "JUMP_IF_TRUE",

    "CALL_NATIVE",
    "CALL_SCRIPT",


    "COUNT",
};

// Função para imprimir um OpCode como string
void printOpCode(OpCode opcode) 
{
    if (opcode >= 0 && opcode < COUNT) 
    {
        
        INFO("%s", OpCodeNames[opcode].c_str());
    } else 
    {

        INFO("Unknown OpCode: %d", opcode);
    }
}


u8 Compiler::Run()
{

// #define READ_BYTE() (*ip++)
// #define READ_SHORT() (ip += 2,(uint16_t)((ip[-2] << 8) | ip[-1]))
// #define READ_CONSTANT() (constants[READ_BYTE()])

Frame* frame = &frames[frameCount - 1]; 


#define READ_BYTE() (*frame->ip++)
#define READ_SHORT()     (frame->ip += 2, \
            (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->compiler->constants[READ_BYTE()])

// for (size_t i = 0; i < constants.size(); i++)
// {
//    // printValue(constants[i]);
// }

// for (size_t i = 0; i < chunk->count; i++)
// {
//     if (chunk->code[i] == OpCode::CONST)
//     {
//         printValue(constants[chunk->code[i + 1]]);
//         i++;
//     } else 
//         printOpCode((OpCode)chunk->code[i]);

// }



//return 0;

Vector<Value> defines;



while (true)
{

    u8 instruction = READ_BYTE();
    int lineIndex = static_cast<int>(  frame->ip - frame->compiler->chunk->code -1);
    int line = frame->compiler->chunk->lines[lineIndex];
    switch ((OpCode)instruction)
    {
        case OpCode::CONST:
        {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
//*************************************************************** */
//BASIC OPERATIONS
//*************************************************************** */

        case OpCode::PUSH:
        {
            Value value = READ_CONSTANT();
            push(value);
            break;
        }
        case OpCode::POP:
        {
            pop();
            break;
        }
        case OpCode::TRUE:
        {
            push(to_boolean(true));
            break;
        }
        case OpCode::FALSE:
        {
            push(to_boolean(false));
            break;
        }
        case OpCode::NIL:
        {
            push(to_none());
            break;
        }

        case OpCode::HALT:
        {
            vm->Warning("Halt!");
            return INTERPRET_HALT;
        }
        case OpCode::DUP:
        {
            Value value = peek(0);
            push(value);
            break;
        }
    case OpCode::RETURN:
    {
        
        Value result = pop();
        frameCount--;
        if (frameCount == 0)
        {
            INFO("return from '%s' ", frame->compiler->name.c_str());
            PrintStack();
            pop();
            return INTERPRET_OK;
        }
        Scope* back = scopeStack.back();
        scopeStack.pop_back();
        Factory::as().DestroyScope(back);
        stackTop = frame->slots;
        push(result);
        frame = &frames[frameCount - 1];
        break;
    }

//*************************************************************** */
//MATH EXPRESSIONS OPERATIONS
//*************************************************************** */
    
        case OpCode::MOD:
        {
             Value b = pop();
             Value a = pop();
            if (is_number(a) && is_number(b))
            {
                int ia = as_integer(a);
                int ib = as_integer(b);
                int result = ia % ib;

                push(std::move(to_integer(result)));
            
            }
            else
            {
                vm->Error("invalid 'mod' operands [line %d]", line);

                return INTERPRET_RUNTIME_ERROR;
            }

            break;
        }
    case OpCode::ADD:
    {

            Value b = pop();
            Value a = pop();
            if (is_number(a) && is_number(b))
            {
                Value result(to_number(as_number(a) + as_number(b)));

                push(std::move(result));
            }
            else if (is_string(a) && is_string(b))
            {
                Chars result = a.string->string;
                result +=b.string->string;
                push(std::move(to_string(result.c_str())));
            }
            else if (is_string(a) && is_number(b))
            {
                Chars number(as_number(b));
                Chars result = a.string->string;
                result += number;
                push(std::move(to_string(result.c_str())));
            }
            else if (is_number(a) && is_string(b))
            {
                Chars number(as_number(a));
                Chars result = number;

                result+= b.string->string;
                push(std::move(to_string(result.c_str())));
            }
            else
            {
                vm->Error("invalid 'adding' operands");
                printValue(a);
                printValue(b);
                return INTERPRET_RUNTIME_ERROR;
            }
        break;
    }
    case OpCode::SUBTRACT:
    {
        Value b = pop();
        Value a = pop();

        if (is_number(a) && is_number(b))
        {
            Value result = to_number(as_number(a) - as_number(b));
            push(std::move(result));
        }
        else
        {
            vm->Error("invalid  'subtract' operands [line %d]", line);

            return  INTERPRET_RUNTIME_ERROR;
        }

      
        break;
    }
    case OpCode::MULTIPLY:
    {
        Value b = pop();
        Value a = pop();
     
        if (is_number(a) && is_number(b))
        {
            Value result = to_number(as_number(a) * as_number(b));
            push(std::move(result));
        }
        else
        {
            vm->Error("invalid 'multiply' operands [line %d]", line);
            return  INTERPRET_RUNTIME_ERROR;
        }
        break;
    }
    case OpCode::DIVIDE:
    {
        Value b = pop();
        Value a = pop();
        if (is_number(a) && is_number(b))
        {
            if (as_number(b) == 0)
            {
                vm->Error("division by zero [line %d]", line);
                return  INTERPRET_RUNTIME_ERROR;
            }
            Value result = to_number(as_number(a) / as_number(b));
            push(std::move(result));
        }
        else
        {
            vm->Error("invalid 'divide' operands [line %d]", line);
            return INTERPRET_RUNTIME_ERROR;
        }
        break;
    }

//*************************************************************** */
//UNARY  EXPRESSIONS OPERATIONS
//*************************************************************** */

    case OpCode::NEGATE:
    {
            Value value = pop();
            if (is_number(value))
            {
                Value result = to_number(-as_number(value));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'negate' operands, Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
        break;
    }
    case OpCode::NOT:
    {
        Value value = pop();
        push(std::move(to_boolean(is_falsey(value))));
        break;
    }

    case OpCode::OR:
    {
        Value b = pop();
        Value a = pop();
        push(std::move(to_boolean(as_boolean(a) || as_boolean(b))));
        break;
    }
    case OpCode::AND:
    {
        Value b = pop();
        Value a = pop();
        push(std::move(to_boolean(as_boolean(a) && as_boolean(b))));
        break;
    }
    case OpCode::XOR:
    {
        Value b = pop();
        Value a = pop();
        push(std::move(to_boolean(as_boolean(a) ^ as_boolean(b))));
        break;
    }

//*************************************************************** */
//COMPARISON EXPRESSIONS OPERATIONS
//*************************************************************** */

     case OpCode::EQUAL:
        {
         
            Value b = pop();
            Value a = pop();
            bool result = is_match(a, b);
            push(std::move(to_boolean(result)));

            break;
        }
        case OpCode::EVAL_EQUAL:
        {
            Value b = peek(0);
            Value a = peek(1);
            bool result = is_match(a, b);
            push(std::move(to_boolean(result)));
            break;
        }
        case OpCode::NOT_EQUAL:
        {
            Value b = pop();
            Value a = pop();
            if (is_number(a) && is_number(b))
            {
                Value result = to_boolean(as_number(a) != as_number(b));
                push(std::move(result));
            }
            else if (is_string(a) && is_string(b))
            {
                Value result = to_boolean(a.string->string != b.string->string);
                push(std::move(result));
            }
            else if (is_boolean(a) && is_boolean(b))
            {
                Value result = to_boolean(as_boolean(a) != as_boolean(b));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'not equal' operands.");
                printValue(a);
                printValue(b);
                return  INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::LESS:
        {
            Value b = pop();
            Value a = pop();
            if (is_number(a) && is_number(b))
            {
                Value result = to_boolean(as_number(a) < as_number(b));
                push(std::move(result));
            }
            else if (is_string(a) && is_string(b))
            {
                Value result = to_boolean(a.string->string.length() < b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'less' operands.");
                printValue(a);
                printValue(b);
                return  INTERPRET_RUNTIME_ERROR;
            }


            break;
        }
        case OpCode::LESS_EQUAL:
        {
             Value b = pop();
            Value a = pop();
            if (is_number(a) && is_number(b))
            {
                Value result = to_boolean(as_number(a) <= as_number(b));
                push(std::move(result));
            }
            else if (is_string(a) && is_string(b))
            {
                Value result = to_boolean(a.string->string.length() <= b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'less equal' .");
                printValue(a);
                printValue(b);

                return   INTERPRET_RUNTIME_ERROR;
            }

            break;
        }
        case OpCode::GREATER:
        {
            Value b = pop();
            Value a = pop();
            if (is_number(a) && is_number(b))
            {
                Value result = to_boolean(as_number(a) > as_number(b));
                push(std::move(result));
            }
            else if (is_string(a) && is_string(b))
            {
                Value result = to_boolean(a.string->string.length() > b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'greater' .");
                printValue(a);
                printValue(b);

                return  INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::GREATER_EQUAL:
        {
            
            Value b = pop();
            Value a = pop();
            if (is_number(a) && is_number(b))
            {
                Value result = to_boolean(as_number(a) >= as_number(b));
                push(std::move(result));
            }
            else if (is_string(a) && is_string(b))
            {
                Value result = to_boolean(a.string->string.length() >= b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'greater equal' .");
                printValue(a);
                printValue(b);

                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }



//*************************************************************** */
//BUILT INS OPERATIONS
//*************************************************************** */


        case OpCode::PRINT:
        {
            Value value = pop();
            printf("%sprint:",CONSOLE_COLOR_YELLOW);
            debugValue(value);
            printf("%s\n",CONSOLE_COLOR_RESET);
            break;
        }
        case OpCode::NOW:
        {
            Value time = to_number(time_now());
            push(std::move(time));
            break;
        }

//*************************************************************** */
//JUMP OPERATIONS
//*************************************************************** */

       
        case OpCode::JUMP_IF_FALSE:
        {
            uint16_t offset = READ_SHORT();
            if (is_falsey(peek(0)))
            {
                frame->ip += offset;
            }
            break;
        }
        case OpCode::JUMP_IF_TRUE:
        {
            uint16_t offset = READ_SHORT();
            if (!is_falsey(peek(0)))
            {
                frame->ip += offset;
            }
            break;
        }
        case OpCode::JUMP:
        {
            uint16_t offset = READ_SHORT();
            frame->ip += offset;
            break;
        }
        case OpCode::LOOP:
        {
            int offset = READ_SHORT();
            frame->ip -= offset;
            break;
        }
        case OpCode::BACK:
        {
            int offset = READ_SHORT();
            frame->ip -= offset;
            break;
        }
        case OpCode::SAVE:
        {
             int adress = static_cast<int>(  frame->ip - frame->compiler->chunk->code - 1);
            push(to_integer(adress));
            INFO("save address: %d", adress);
            break;
        }
        case OpCode::RESTORE:
        {
            Value value = pop();
            int adress = as_integer(value);
            frame->ip = frame->compiler->chunk->code + adress - 1;
            INFO("restor address: %d", adress);
            break;
        }

//*************************************************************** */
//OPERATORS OPERATIONS
//*************************************************************** */

        case ::INC:
        {
            Value name = pop();
            Chars  nameStr = as_string(name);
            Scope *scope = scopeStack.back();
            Value value;
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value))
                {
                    double _value = as_number(value);
                    ++_value;
                    value = to_number(_value);
                    scope->assign(nameStr, std::move(value));
                    push(value);
                }
                else
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }

            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::DEC:
        {
            Value name = pop();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value))
                {
                    double _value = as_number(value);
                    --_value;
                    value = to_number(_value);
                    scope->assign(nameStr, std::move(value));
                    push(value);
                } else 
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::RINC:
        {
            Value name = READ_CONSTANT();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value))
                {
                    push(value);
                    double _value = as_number(value);
                    _value++;
                    value = to_number(_value);
                    scope->assign(nameStr, std::move(value));
                    
                }
                else
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }

            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::RDEC:
        {
            Value name = READ_CONSTANT();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value))
                {
                    push(value);
                    double _value = as_number(value);
                    _value--;
                    value = to_number(_value);
                    scope->assign(nameStr, std::move(value));
                    
                } else 
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::OPADD:
        {
            Value name = READ_CONSTANT();
            Value action = pop();
            pop();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value) && is_number(action))
                {
                    double _value = as_number(value);
                    double _action = as_number(action);
                    value = to_number(_value + _action);
                    scope->assign(nameStr, std::move(value));
             
                }
                else
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::OPSUB:
        {
            Value name = READ_CONSTANT();
            Value action = pop();
            pop();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value) && is_number(action))
                {
                    double _value = as_number(value);
                    double _action = as_number(action);
                    value = to_number(_value - _action);
                    scope->assign(nameStr, std::move(value));
                } else 
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::OPMUL:
        {
            Value name = READ_CONSTANT();
            Value action = pop();
            pop();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value) && is_number(action))
                {
                    double _value = as_number(value);
                    double _action = as_number(action);
                    value = to_number(_value * _action);
                    scope->assign(nameStr, std::move(value));
                } else 
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::OPDIV:
        {
            Value name = READ_CONSTANT();
            Value action = pop();
            pop();
            Chars  nameStr = as_string(name);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(nameStr, value))
            {
                if (is_number(value) && is_number(action))
                {
                    double _value = as_number(value);
                    double _action = as_number(action);
                    if (_action == 0)
                    {
                        vm->Error("Divide by zero [line %d]", line);
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    value = to_number(_value / _action);
                    scope->assign(nameStr, std::move(value));
                } else 
                {
                    vm->Error("Variable '%s' is not a number [line %d]", nameStr.c_str(), line);
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            else
            {
                vm->Error("Undefined variable '%s' [line %d]", nameStr.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }

//*************************************************************** */
//VARIABLE OPERATIONS
//*************************************************************** */


         case OpCode::GLOBAL_DEFINE:
        {

            //vm->Warning("Variable '%s'  defined [line %d]", name.c_str(), line);

            Value constant = READ_CONSTANT();
            Chars name = as_string(constant);
            Value value = pop();
            Scope *scope = scopeStack.back();
            if (!scope->define(name, value))
            {
                vm->Error("Variable '%s' already defined [line %d]", name.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }

            //printValue(value);
          //  scope->print();

           break;
        }
        case OpCode::GLOBAL_SET:
        {

         

            // Scope *scope = scopeStack.back();
            // Value value = pop();
            // while (defines.size() > 0)
            // {
            //     Value name = defines.back();
            //     String nameStr = as_string(name)->string;
            //     defines.pop_back();
            //     scope->define(nameStr, std::move(value));
            // }
            // defines.clear();
            break;
        }
        case OpCode::GLOBAL_GET:
        {
            Value vName      = READ_CONSTANT();
            Chars name = as_string(vName);
            Value value;
            Scope *scope = scopeStack.back();
            if (scope->lookup(name, value))
            {
                push(value);
            } else 
            {
                vm->Error("Undefined variable '%s' [line %d]", name.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::GLOBAL_ASSIGN:
        {
            
            Value vName = READ_CONSTANT();
            Value value =pop();
          //  printValue(vName);
           // printValue(value);
            Chars name = as_string(vName);
            Scope *scope = scopeStack.back();
            if (!scope->assign(name, value))
            {
                vm->Error("Undefined variable '%s' [line %d]", name.c_str(), line);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OpCode::ENTER_SCOPE:
        {
            Scope *scope = scopeStack.back();
            Scope* block = Factory::as().CreateScope(scope);
            scopeStack.push_back(block);
      
           // INFO("Enter scope %p", block);
            break;
        }
        case OpCode::EXIT_SCOPE:
        {
            
            Scope* back = scopeStack.back();
            scopeStack.pop_back();
            Factory::as().DestroyScope(back);
          //  INFO("Exit scope %p", back);
       
  
            break;
        }
//*************************************************************** */
//CALLS OPERATIONS
//*************************************************************** */

//         case OpCode::CALL_SCRIPT:
//         {
//             u8 argCount = READ_BYTE();
//             Value nameValue = peek(argCount);
//             String name = as_string(nameValue)->string;
//             Compiler *task = vm->getCompiler(name);

//             if (!task)
//             {
//                 vm->Error("Undefined function '%s' [line %d]", name.c_str(), line);
//                 return INTERPRET_RUNTIME_ERROR;
//             }
           

//             if (task->argsCount() != argCount)
//             {
//                 vm->Error("Expected %d arguments in call to '%s', but got %d [line %d]", task->argsCount(), name.c_str(), argCount, line);
//                 return INTERPRET_RUNTIME_ERROR;
//             }
//            // Factory::as().DestroyScope(task->local);

      
//             task->createScope(scopeStack.back());
     
//             scopeStack.push_back(task->local);

          
           
//             int index = argCount-1;
//             for (u8 i = 0; i < argCount; i++)
//             {
//                 task->local->define(task->args[i],peek(index--));
//             }
//           //  task->local->setParent(scope);

           

          

//              frame = &frames[frameCount++];
//              frame->compiler = task;
//              frame->ip = task->chunk->code;
//              frame->slots = stackTop       -argCount - 1;
//              pop(argCount +1);




//             if (frameCount == MAX_FRAMES)
//             {
//                 vm->Error("Frames overflow [line %d]", line);
//                 return INTERPRET_RUNTIME_ERROR;
//             }


//           //  PrintStack();

//            // task->Disassemble();

//            // INFO("Calling script %s", name.c_str());
//             break;
//         }



        default:
        {
            vm->Error(" %s running %d with unknown '%d' opcode frame %d", name.c_str(), frame->ip, (int)instruction, frameCount);

            return INTERPRET_RUNTIME_ERROR;
        }
    }//switch
}//while

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT



    return INTERPRET_OK;
}
void Compiler::Disassemble()
{
    disassembleCode(name.c_str());
}
    