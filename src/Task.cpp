

#include "pch.h"
#include "Vm.hpp"
#include "Parser.hpp"

static u64 nextID = 0;




void debugValue(const Value &v)
{
    switch (v.type)
    {
        case ValueType::VSTRING:  printf("%s", v.string->string.c_str()); break;
        case ValueType::VNUMBER:  printf("%g", v.number); break;
        case ValueType::VBOOLEAN: printf("%s", v.boolean ? "true" : "false"); break;
        case ValueType::VNONE:    printf("nil"); break;
        case ValueType::VNATIVE:   printf("<native %s>", v.native->name.c_str()); break;

        default: printf("Unknow value "); break;
    }
}
void printValueln(const Value &v)
{
    switch (v.type)
    {
        case ValueType::VSTRING:  printf("%s\n", v.string->string.c_str()); break;
        case ValueType::VNUMBER:  printf("%g\n", v.number); break;
        case ValueType::VBOOLEAN: printf("%s\n", v.boolean ? "true" : "false"); break;
        case ValueType::VNONE:    printf("nil\n"); break;
        case ValueType::VNATIVE:   printf("<native %s>\n", v.native->name.c_str()); break;
      
        default: printf("Unknow value "); break;
    }
}

void printValue(const Value &v)
{
    switch (v.type)
    {
        case ValueType::VSTRING:  PRINT("%s", v.string->string.c_str()); break;
        case ValueType::VNUMBER:  PRINT("%g", v.number); break;
        case ValueType::VBOOLEAN: PRINT("%s", v.boolean ? "true" : "false"); break;
        case ValueType::VNONE:    PRINT("nil"); break;
        case ValueType::VNATIVE:  PRINT("<native %s>", v.native->name.c_str()); break;

        default: PRINT("Unknow value "); break;
    }
}





void Task::exitScope()
{
   
  
}



Task::Task(VirtualMachine *vm, const char *name):Traceable()
{
    m_done = false;
    this->vm = vm;
    ID = nextID++;
    type = ObjectType::OTASK;
    parent = nullptr;
    argsCount = 0;
    
    this->name = name;
    constants.reserve(256);
    frameDepth = 1024;
    code.reserve(1024);
    lines.reserve(code.capacity());

    local.name = name;
    local.level = ID;
    local.parent = vm->global;  

    ip = &code[0];
    lastIp = &code[0];
    lineBuffer = &lines[0];
    stackTop = stack;

}

Task::~Task()
{
    constants.clear();
    code.clear();
    lines.clear();
    local.clear();
   // INFO("Free task %s with id %d", name,ID);

}

void Task::reset()
{
    ip = &code[0];
    lastIp = ip;
    lineBuffer = &lines[0];
    stackTop = stack;
}

void Task::write_byte(u8 byte, int line)
{
    
    
    code.push_back(byte);
    lines.push_back(line);

    
}

bool Task::addArgs(const String &name)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        if (matchString(args[i].c_str(),name.c_str(),name.size()))
        {
            return false;
        }
    }
    args.push_back(name);
    argsCount++;
    return true;
}

u8 Task::addConst(Value v)
{
    constants.push_back(std::move(v));
    return (u8)constants.size() - 1;
}

u8 Task::addConstString(const char *str)
{
    return addConst(STRING(str));
}

u8 Task::addConstNumber(double number)
{
    return addConst(NUMBER(number));
}

void Task::writeByte(u8 byte,int line)
{
    write_byte(byte, line);
}

void Task::writeBytes(u8 byte1, u8 byte2,int line)
{
    writeByte(byte1,line);
    writeByte(byte2,line);
}
void Task::writeConstant(const Value &value,int line)
{
    writeBytes(OpCode::CONST, makeConstant(std::move(value)), line);
}
u8 Task::makeConstant( Value value)
{
    int constant = (int)addConst(std::move(value));
    if (constant > 255)
    {
        vm->Error("Too many constants in  task");
        return 0;
    }
    return (u8)constant;
}

void Task::writeConstantVar(const char *name, Value value,int line)
{
    writeBytes(OpCode::CONST, makeConstant(std::move(value)), line);
    writeBytes(OpCode::CONST, addConstString(name), line);
}




void Task::patch(u32 offset)
{
    while (true) 
    {
        size_t count = code.size();
        int jump = count - offset - 2;
        if (jump > UINT16_MAX) 
        {
            vm->Error("Too much code to jump over.");
            break;
        }

        if (offset + 1 >= count) 
        {
            vm->Error("[PATCH] Offset out of bounds.");
            break;
        }

        int next = (code[offset] << 8) | code[offset + 1];

        code[offset] = (jump >> 8) & 0xff;
        code[offset + 1] = jump & 0xff;

        if (next == UINT16_MAX)
            break;

        offset += next;
    }
}

 void Task::addToInPlaceJumpOffsetList(int offset, int jumpAddress) 
 {
	// Skip to end of list. Could be optimized away by directly storing the end of the list in the compiler struct:
    int count =(int) code.size();
	while (true) 
    {
         if (offset + 1 >= count) 
        {
            vm->Error("[JUMP OFFSET] Offset out of bounds.");
            return;
        }

		int next = (code[offset] << 8) | code[offset + 1];
		if (next == UINT16_MAX)
			break;
		offset += next;
	}

	int jump = jumpAddress - offset;
	if (jump >= UINT16_MAX) 
    {
		vm->Error("[JUMP OFFSET] Too much code for offset.");
        return;
    }
   
    if (offset + 1 >= count) 
    {
        vm->Error("[JUMP OFFSET] Offset out of bounds.");
        return;
    }

    // Append new offset from previous jump to current jump to the list:
    code[offset]   = (jump >> 8) & 0xff;
    code[offset+1] = jump & 0xff;
}

void Task::go_to(int offset, u8 jump)
{
    if (offset < 0 || offset >= (int)code.size())
    {
        vm->Warning("[GO TO] Offset out of bounds.");
        
        return;
    }
    code[offset] = jump;
}

void Task::patchBreaks(int breakJump)
{
    while (breakJump != -1) 
    {
        int jump = breakJump;
        breakJump = code[jump];
        patch(jump);//patch jump
    }
}
u32 Task::code_size()
{
    return code.size();
}

void Task::disassembleCode(const char *name)
{
     
    printf("================== %s ==================\n", name);
    printf("\n");
    for (u32 offset = 0; offset < code.size();)
    {
        offset = disassembleInstruction(offset);
    }
    printf("\n");
}

u32 Task::disassembleInstruction(u32 offset)
{
    
    printf("%04d ", offset);
    if (offset > 0 && lines[offset] == lines[offset - 1])
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", lines[offset]);
    }
    u8 instruction = code[offset];
    switch ((OpCode)instruction)
    {
        case OpCode::CONST:  return constantInstruction("CONSTANT", offset);
        case OpCode::POP:    return simpleInstruction("POP", offset);
        case OpCode::PUSH:   return simpleInstruction("PUSH", offset);
        case OpCode::RETURN: return simpleInstruction("RETURN", offset);
        case OpCode::HALT:   return simpleInstruction("HALT", offset);
        case OpCode::PRINT:  return simpleInstruction("PRINT", offset);
        case OpCode::NOW:    return simpleInstruction("NOW", offset);
        case OpCode::FRAME:   return simpleInstruction("FRAME", offset);
        case OpCode::ADD:    return simpleInstruction("ADD", offset);
        case OpCode::SUBTRACT:    return simpleInstruction("SUBTRACT", offset);
        case OpCode::MULTIPLY:    return simpleInstruction("MULTIPLY", offset);
        case OpCode::DIVIDE:    return simpleInstruction("DIVIDE", offset);
        case OpCode::MOD:    return simpleInstruction("MOD", offset);
        case OpCode::POWER:    return simpleInstruction("POWER", offset);
        case OpCode::NEGATE:    return simpleInstruction("NEGATE", offset);
        case OpCode::EQUAL:    return simpleInstruction("EQUAL", offset);
        case OpCode::NOT_EQUAL:    return simpleInstruction("NOT_EQUAL", offset);
        case OpCode::GREATER:    return simpleInstruction("GREATER", offset);
        case OpCode::GREATER_EQUAL:    return simpleInstruction("GREATER_EQUAL", offset);
        case OpCode::LESS:    return simpleInstruction("LESS", offset);
        case OpCode::LESS_EQUAL:    return simpleInstruction("LESS_EQUAL", offset);
        case OpCode::NOT:    return simpleInstruction("NOT", offset);
        case OpCode::XOR:    return simpleInstruction("XOR", offset);
        case OpCode::SHL:    return simpleInstruction("SHL", offset);
        case OpCode::SHR:    return simpleInstruction("SHR", offset);

        case OpCode::LOOP_BEGIN:    return simpleInstruction("LOOP_BEGIN", offset);
        case OpCode::LOOP_END:    return simpleInstruction("LOOP_END", offset);

        case OpCode::BREAK:    return simpleInstruction("BREAK", offset);
        case OpCode::CONTINUE:    return simpleInstruction("CONTINUE", offset);


        case OpCode::DUP:    return simpleInstruction("DUP", offset);
        case OpCode::EVAL_EQUAL:    return simpleInstruction("EVAL_EQUAL", offset);


        case OpCode::ENTER_SCOPE:    return simpleInstruction("ENTER_SCOPE", offset);
        case OpCode::EXIT_SCOPE:    return simpleInstruction("EXIT_SCOPE", offset);


        case OpCode::VARIAVEL_DEFINE:    return varInstruction("VARIAVEL_DEFINE", offset);
        case OpCode::VARIAVEL_ASSIGN:    return varInstruction("VARIAVEL_ASSIGN", offset);
        case OpCode::VARIAVEL_GET:    return varInstruction("VARIAVEL_GET", offset);
        
        
       

        case OpCode::SWITCH:    return simpleInstruction("SWITCH", offset);
        case OpCode::CASE:    return jumpInstruction("CASE",1, offset);
        case OpCode::SWITCH_DEFAULT:    return jumpInstruction("DEFAULT", 1,offset);





        case OpCode::JUMP_BACK:    return jumpInstruction("JUMP_BACK", -1,  offset);
        
        case OpCode::JUMP:    return jumpInstruction("JUMP", 1,  offset);
        case OpCode::JUMP_IF_FALSE:   return jumpInstruction("JUMP_IF_FALSE", 1,  offset);
        case OpCode::JUMP_IF_TRUE:  return  jumpInstruction("JUMP_IF_TRUE", 1,  offset);

        case OpCode::CALL:    return byteInstruction("CALL_NATIVE", offset);
        case OpCode::CALL_SCRIPT:    return byteInstruction("CALL_SCRIPT", offset);
        case OpCode::CALL_PROCESS:    return byteInstruction("CALL_PROCESS", offset);

        case OpCode::RETURN_DEF:    return byteInstruction("DEF_RETURN", offset);
        case OpCode::RETURN_PROCESS:    return byteInstruction("PROCESS_RETURN", offset);
        case OpCode::RETURN_NATIVE:    return byteInstruction("NATIVE_RETURN", offset);
        
        
       
        case OpCode::NIL :    return byteInstruction("NIL", offset);

        
        default: 
        {
            printf("Unknow instruction %d\n", instruction); 
            return code.size();
            break;
        }
    }
    return offset;
}

u32 Task::byteInstruction(const char *name, u32 offset)
{
    u8 slot = code[offset+1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

u32 Task::jumpInstruction(const char *name, u32 sign, u32 offset)
{
    u16 jump = (u16)code[offset+1] << 8;
    jump |= code[offset+2];
    printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

u32 Task::varInstruction(const char *name, u32 offset)
{
    u8 constant = code[offset+1];


    


    printf("%-16s %4d '", name, constant);
    Value value = constants[constant];
    debugValue(value);
    printf("'\n");

  
    
    
  
    
    return offset + 2;
}

u32 Task::constantInstruction(const char *name, u32 offset)
{
   
    
    u8 constant = code[offset+1];


    


    printf("%-16s %4d '", name, constant);
    Value value = constants[constant];

    

    

    debugValue(value);
    printf("'\n");


    return offset + 2;
}

u32 Task::simpleInstruction(const char *name, u32 offset)
{
    printf("%s\n", name);
    return offset + 1;
}





Value Task::top()
{
    return *stackTop;
}

bool Task::push(Value v)
{
    if (stackTop - stack >= STACK_MAX)
    {
        PrintStack();
        vm->Error("[PUSH] Stack overflow %s",name.c_str());
        return false;
    }
    *stackTop = std::move(v);
    stackTop++;
    return true;
}

Value Task::pop()
{
    if (stackTop == stack)
    {
      //  vm->Error("[POP] Stack underflow");
        return NONE();
    }

    stackTop--;
    return std::move(*stackTop);
}

Value Task::peek(int distance)
{
    if (distance<0 || stackTop - stack <= distance)
    {
        vm->Error("[PEEK] Stack underflow at distance %d", distance);
        return NONE();
    }
    return *(stackTop - 1 - distance);
}

void Task::pop(u32 count)
{
    for (u32 i = 0; i < count; i++)
    {
        pop();
    }
}

int Task::read_line()
{
    return *lineBuffer;
}


void Task::PrintStack()
{
    printf("          ");
    for (Value *slot = stack; slot < stackTop; slot++)
    {
        printf("[ ");
        debugValue(*slot);
        printf("] ");
    }

    printf("\n");
}
u8 Task::read_byte()
{
     return *ip++;
    
}

u16 Task::read_short()
{
    return (u16)read_byte() << 8 | read_byte();
    // ip += 2;
    // return static_cast<uint16_t>((currentTask->code[ip - 2] << 8) | currentTask->code[ip - 1]);
    // *ip += 2;
    // return static_cast<uint16_t>((ip[-2] << 8) | ip[-1]);
}



Value Task::read_const()
{
    u8 index = read_byte();
    if (index >= constants.size())
    {
        vm->Warning("Invalid constant index");
     
        return NONE();
    }
    return constants[index];
}

Value Task::read_const(int index)
{
    if (index<0 || index >=(int) constants.size())
    {
        vm->Warning("Invalid constant index");
     
        return NONE();
    }
    return constants[index];
}



bool Task::IsDone()
{
    return m_done;
}

void Task::Done()
{
    m_done = true;
}

//***************************************************************************************************************** */
//***************************************************************************************************************** */
//***************************************************************************************************************** */
// bool VirtualMachine::RunTask(Task *task)
// {
//     (void)task;
//     return false;
// }

 static const size_t instructionsPerFrame = 120;
u8 Task::Run()
{
  
     size_t instructionsExecuted = 0;
  

    //for (;;)
    while(instructionsExecuted < instructionsPerFrame)
    {
    

        u8 instruction = read_byte();
        int line = read_line();

        if (instruction >= COUNT)
        {
            //ip = &code[0];
            //lineBuffer = &lines[0];
             vm->Warning("Unknown instruction ");
            return TERMINATED;
        }

    switch ((OpCode)instruction)
    {
        case OpCode::CONST:
        {

            Value value = read_const();
            push(std::move(value));

            break;
        }
        case OpCode::PUSH:
        {

            Value value = read_const();
            push(std::move(value));
            break;
        }
        case OpCode::POP:
        {
            pop();
            break;
        }

        case OpCode::HALT:
        {
            vm->Warning("Halt!");
            return ABORTED;
        }
        case OpCode::PRINT:
        {
            Value value = pop();
            // printValue(std::move(value));
            printValueln(std::move(value));
            break;
        }
        case OpCode::NOW:
        {
            push(std::move(NUMBER(time_now())));
            break;
        }
        case OpCode::ADD:
        {
             Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {

                Value result = NUMBER(AS_NUMBER(a) + AS_NUMBER(b));
                push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                String result = a.string->string + b.string->string;
                push(std::move(STRING(result.c_str())));
            }
            else if (IS_STRING(a) && IS_NUMBER(b))
            {
                String number(AS_NUMBER(b));
                String result = a.string->string + number;
                push(std::move(STRING(result.c_str())));
            }
            else if (IS_NUMBER(a) && IS_STRING(b))
            {
                String number(AS_NUMBER(a));
                String result = number + b.string->string;
                push(std::move(STRING(result.c_str())));
            }
            else
            {

                vm->Error("invalid 'adding' operands [line %d]", line);
                return ABORTED;
            }
            break;
        }
        case OpCode::SUBTRACT:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = NUMBER(AS_NUMBER(a) - AS_NUMBER(b));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid  'subtract' operands [line %d]", line);
                
                return ABORTED;
            }

            break;
        }
        case OpCode::MULTIPLY:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = NUMBER(AS_NUMBER(a) * AS_NUMBER(b));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'multiply' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::DIVIDE:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                if (AS_NUMBER(b) == 0)
                {
                    vm->Error("division by zero [line %d]", line);
                    
                    return ABORTED;
                }
                Value result = NUMBER(AS_NUMBER(a) / AS_NUMBER(b));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'divide' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::MOD:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                double _a = AS_NUMBER(a);
                double divisor = AS_NUMBER(b);
                if (divisor == 0)
                {
                    vm->Error("division by zero [line %d]", line);
                    
                    return ABORTED;
                }
                double result = fmod(_a, divisor);
                if (result != 0 && ((_a < 0) != (divisor < 0)))
                {
                    result += divisor;
                }
                push(std::move(NUMBER(result)));
            }
            else
            {
                vm->Error("invalid 'mod' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::POWER:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = NUMBER(pow(AS_NUMBER(a), AS_NUMBER(b)));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'power' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::NEGATE:
        {
            Value value = pop();
            if (IS_NUMBER(value))
            {
                Value result = NUMBER(-AS_NUMBER(value));
                push(std::move(result));
            }
            else if (IS_BOOLEAN(value))
            {
                Value result = BOOLEAN(!AS_BOOLEAN(value));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'negate' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::NOT:
        {
            Value value = pop();
            if (IS_BOOLEAN(value))
            {
                Value result = BOOLEAN(!AS_BOOLEAN(value));
                push(std::move(result));
            }
            else if (IS_NUMBER(value))
            {
                Value result = BOOLEAN(AS_NUMBER(value) == 0);
                push(std::move(result));
            }

            else
            {
                vm->Error("invalid 'not' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::EQUAL:
        {
            Value b = pop();
            Value a = pop();
            bool result = MatchValue(a, b);
            push(BOOLEAN(result));

            break;
        }
        case OpCode::EVAL_EQUAL:
        {
            Value b = peek(0);
            Value a = peek(1);
            bool result = MatchValue(a, b);
            push(BOOLEAN(result));
            break;
        }
        case OpCode::NOT_EQUAL:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) != AS_NUMBER(b));
                push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string != b.string->string);
                push(std::move(result));
            }
            else if (IS_BOOLEAN(a) && IS_BOOLEAN(b))
            {
                Value result = BOOLEAN(AS_BOOLEAN(a) != AS_BOOLEAN(b));
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'not equal' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::LESS:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) < AS_NUMBER(b));
                push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() < b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'less' operands [line %d]", line);
                
                return ABORTED;
            }

            break;
        }
        case OpCode::LESS_EQUAL:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) <= AS_NUMBER(b));
                push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() <= b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'less equal'   [line %d]", line);
                
                return ABORTED;
            }

            break;
        }
        case OpCode::GREATER:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) > AS_NUMBER(b));
                push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() > b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'greater'  [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::GREATER_EQUAL:
        {
            Value b = pop();
            Value a = pop();
            if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                Value result = BOOLEAN(AS_NUMBER(a) >= AS_NUMBER(b));
                push(std::move(result));
            }
            else if (IS_STRING(a) && IS_STRING(b))
            {
                Value result = BOOLEAN(a.string->string.length() >= b.string->string.length());
                push(std::move(result));
            }
            else
            {
                vm->Error("invalid 'greater equal'    [line %d]", line);
                
                return ABORTED;
            }
            break;
        }

        case OpCode::XOR:
        {
            Value b = pop();
            Value a = pop();
            if (IS_BOOLEAN(a) && IS_BOOLEAN(b))
            {
                bool b_a = AS_BOOLEAN(a);
                bool b_b = AS_BOOLEAN(b);
                bool result = b_a ^ b_b;
                push(std::move(BOOLEAN(result)));
            }
            else if (IS_NUMBER(a) && IS_NUMBER(b))
            {
                int n_a = static_cast<int>(AS_NUMBER(a));
                int n_b = static_cast<int>(AS_NUMBER(b));
                int result = n_a ^ n_b;
                double number = static_cast<double>(result);
                push(std::move(NUMBER(number)));
            }
            else if (IS_BOOLEAN(a) && IS_NUMBER(b))
            {
                bool b_a = AS_BOOLEAN(a);
                int n_b = static_cast<int>(AS_NUMBER(b));
                bool result = b_a ^ n_b;
                push(std::move(BOOLEAN(result)));
            }
            else if (IS_NUMBER(a) && IS_BOOLEAN(b))
            {
                int n_a = static_cast<int>(AS_NUMBER(a));
                bool b_b = AS_BOOLEAN(b);
                bool result = n_a ^ b_b;
                push(std::move(BOOLEAN(result)));
            }
            else
            {
                vm->Error("invalid 'xor' operands [line %d]", line);
                
                return ABORTED;
            }
            break;
        }
        case OpCode::VARIAVEL_DEFINE:
        {

            Value constant = read_const();
            if (!IS_STRING(constant))
            {
                vm->Error("Variable  name must be string [line %d]", line);
                
                return ABORTED;
            }
            const char *name = AS_RAW_STRING(constant);
            Value value = pop();

            if (vm->getScopeDepth() == 0)
            {
                if (!vm->global->define(name, std::move(value)))
                {
                    vm->Warning("Already a global variable with '%s' name.", name);
                    break;
                }
            }
            else
            {
                //   PRINT("Added local variable %s\n", name);
                if (!local.define(name, std::move(value)))
                {
                    vm->Warning("Already a variable with '%s' name.", name);
                    break;
                }
            }
            //  printValue(value);

            break;
        }

        case OpCode::VARIAVEL_ASSIGN:
        {
            Value constant = read_const();
            if (!IS_STRING(constant))
            {
                vm->Error("Variable  name must be string [line %d]", line);
                
                return ABORTED;
            }
            const char *name = AS_RAW_STRING(constant);
            Value value = peek();

            if (!local.assign(name, value))
            {
                if (!vm->global->assign(name, value))
                {
                    vm->Warning("Undefined variable '%s' [line %d]", name, line);
                }
            }

            break;
        }
        case OpCode::VARIAVEL_GET:
        {
            Value constant = read_const();
            if (!IS_STRING(constant))
            {
                vm->Error("Variable  names must be string [line %d]", line);
                
                return ABORTED;
            }
            const char *name = AS_RAW_STRING(constant);
            Value value;

            if (local.get(name, value))
            {

                push(value);
                break;
            }
            else
            {
                if (vm->global->get(name, value))
                {
                    push(value);
                }
                else
                {
                    vm->Warning("[READ] Undefined variable '%s' [line %d] %d", name, line, vm->getScopeDepth());

                    push(std::move(NONE()));
                }
            }

            break;
        }

        case OpCode::JUMP_IF_FALSE:
        {
            u16 offset = read_short();
            Value condition = pop();

            if (isFalsey(condition))
            {
                ip += offset;
            }
            break;
        }
        case OpCode::JUMP_IF_TRUE:
        {
            u16 offset = read_short();
            Value condition = pop();
            if (!isFalsey(condition))
            {
                ip -= offset;
            }
            break;
        }
        case OpCode::DROP:
        {

            break;
        }
        case OpCode::JUMP:
        {
            u16 offset = read_short();
            ip += offset;
            break;
        }
        case OpCode::DUP:
        {
            Value value = peek(0);
            push(value);
            break;
        }
        case OpCode::JUMP_BACK:
        {
            uint16_t offset = read_short();
            ip -= offset;
            break;
        }

        case OpCode::ENTER_SCOPE:
        {

            
            vm->beginScope();

            // if (type == ObjectType::OPROCESS)
            // {
            //     create();
            // }
           

            break;
        }
        case OpCode::EXIT_SCOPE:
        {

            vm->endScope();
            exitScope();
           

            break;
        }
        case OpCode::CALL:
        {
           // PrintStack();
            size_t argCount = (size_t)read_byte();
            Value func = peek(argCount);
              if (!IS_STRING(func))
            {
                vm->Error("Native Function name must be string [line %d]", line);
                printValue(func);   
                return ABORTED;
            }
            Value args[256];
            size_t index = argCount - 1;
            for (size_t i = 0; i < argCount; i++)
            {
                args[index] = std::move(peek(argCount - i - 1));
                index--;
            }
           
          // PRINT("Calling %s with %d arguments\n", AS_RAW_STRING(func), argCount);
            Value result;
            vm->currentTask = this;
            int count = vm->callNativeFunction(AS_RAW_STRING(func), args, argCount);
            if (count >= 1)
            {
                 result =  pop();
            }
            
            pop(argCount + 1);

            if (count >= 1)
            {
                push(std::move(result));
                 //pop();
            }
           
         
            break;
        }
        case OpCode::CALL_SCRIPT:
        {

            int argCount = (int)read_byte();
            Value func = peek(argCount);
            if (!IS_STRING(func))
            {
                vm->Error("Function name must be string [line %d]", line);
                printValue(func);   
                return ABORTED;
            }

            Task *callTask = vm->getTask(AS_RAW_STRING(func));
            if (!callTask)
            {
                pop(argCount);
                vm->Error("Task '%s' not defined [line %d]", AS_RAW_STRING(func), line);
                
                return ABORTED;
            }
            if (callTask->argsCount != argCount)
            {
                vm->Error("Function %s, Expected %ld arguments but got %ld [line %d]", AS_RAW_STRING(func), callTask->argsCount, argCount, line);
                
                return ABORTED;
            }
            Task *process = vm->addTask(AS_RAW_STRING(func));
            for (int i = argCount - 1; i >= 0; i--)
            {
                Value arg = pop();
                const char *name = callTask->args[i].c_str();
                process->local.add(name, std::move(arg));
            }

         //  Arena::as().add(process);
            vm->run_process.push_back(process);
            process->code = callTask->code;
            process->lines = callTask->lines;
            process->constants = callTask->constants;
            process->reset();
            //   process->disassembleCode(process->name.c_str());
           // pop(); // drop the function name
            process->parent = this;
           

            break;
        }
        case OpCode::RETURN:
        {

            if (matchString("__main__", name.c_str(),name.length()))
            {
                INFO("RETURN %s done end execution", name.c_str());
                return TERMINATED;
            }

            //  PrintStack();
            Value result = pop();
            if (parent != nullptr)
            {
                INFO("Function RETURN %s parent %s", name.c_str(),parent->name.c_str());
                parent->push(std::move(result));
                parent = nullptr;
            }

            

            break;
        }
        case OpCode::CALL_PROCESS:
        {

           // PrintStack();
            u8 argCount = read_byte();
            Value func = peek(argCount);
            if (!IS_STRING(func))
            {
                vm->Warning("Process name must be a string");
                printValue(func);
                
                return ABORTED;
            }
            Task *callTask = vm->getTask(AS_RAW_STRING(func));
            if (!callTask)
            {
                pop(argCount);
                vm->Error("Process '%s' not defined [line %d]", AS_RAW_STRING(func), line);
                
                return ABORTED;
            }
            if (callTask->argsCount != argCount)
            {
                vm->Error("Process %s, Expected %ld arguments but got %ld [line %d]", AS_RAW_STRING(func), callTask->argsCount, argCount, line);
                
                return ABORTED;
            }
            Task *process = vm->AddProcess(AS_RAW_STRING(func));
            for (int i = argCount - 1; i >= 0; i--)
            {
                Value arg = pop();
                const char *name = callTask->args[i].c_str();
                process->local.add(name, std::move(arg));
            }
            pop();
            vm->run_process.push_back(process);
            process->stackTop = process->stack;
            process->code = callTask->code;
            process->lines = callTask->lines;
            process->constants = callTask->constants;
            process->reset();
            process->parent = this;
     

            break;
        }
        case OpCode::RETURN_PROCESS:
        {
            if (parent != nullptr)
            {
                
                if (type == ObjectType::OPROCESS)
                {
                    remove();
                }
                 
                 INFO("Process RETURN %s parent %s", name.c_str(),parent->name.c_str());
    
            }

            return TERMINATED;
        }
        case OpCode::RETURN_NATIVE:
        {
            //INFO("RETURN %s done", name.c_str());
            //PrintStack();
            //pop();
            break;
        }
        case OpCode::FRAME:
        {
            INFO("FRAME %s", name.c_str());
            

            return RUNNING;
        }

        case OpCode::NIL:
        {
            push(std::move(NONE()));
            break;
        }


        default:
        {
            vm->Error(" %s running %d with unknown '%d' opcode ", name.c_str(), ip, (int)instruction);
            
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
