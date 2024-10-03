#include "pch.h"
#include "Utils.hpp"
#include "Interpreter.hpp"



extern void debugValue(const Value& value);


void Compiler::disassembleCode(const char *name)
{

    printf("================== %s ==================\n", name);
    printf("\n");
    for (u32 offset = 0; offset < chunk->count;)
    {
        offset = disassembleInstruction(offset);
    }
    printf("\n");
}

u32 Compiler::disassembleInstruction(u32 offset)
{

    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", chunk->lines[offset]);
    }
    u8 instruction = chunk->code[offset];
    switch ((OpCode)instruction)
    {

    case OpCode::TRUE:
        return simpleInstruction("TRUE", offset);
    case OpCode::NIL:
        return simpleInstruction("NIL", offset);
    case OpCode::FALSE:
        return simpleInstruction("FALSE", offset);
    case OpCode::CONST:
        return constantInstruction("CONSTANT", offset);
    case OpCode::PROGRAM:
        return constantInstruction("PROGRAM", offset);
        
    case OpCode::POP:
        return simpleInstruction("POP", offset);
    case OpCode::PUSH:
        return simpleInstruction("PUSH", offset);
    case OpCode::RETURN:
        return simpleInstruction("RETURN", offset);
    case OpCode::HALT:
        return simpleInstruction("HALT", offset);
    case OpCode::PRINT:
        return simpleInstruction("PRINT", offset);
    case OpCode::NOW:
        return simpleInstruction("NOW", offset);
    case OpCode::FRAME:
        return simpleInstruction("FRAME", offset);
    case OpCode::ADD:
        return simpleInstruction("ADD", offset);
    case OpCode::SUBTRACT:
        return simpleInstruction("SUBTRACT", offset);
    case OpCode::MULTIPLY:
        return simpleInstruction("MULTIPLY", offset);
    case OpCode::DIVIDE:
        return simpleInstruction("DIVIDE", offset);
    case OpCode::MOD:
        return simpleInstruction("MOD", offset);
    case OpCode::NEGATE:
        return simpleInstruction("NEGATE", offset);
    case OpCode::EQUAL:
        return simpleInstruction("EQUAL", offset);
    case OpCode::NOT_EQUAL:
        return simpleInstruction("NOT_EQUAL", offset);
    case OpCode::GREATER:
        return simpleInstruction("GREATER", offset);
    case OpCode::GREATER_EQUAL:
        return simpleInstruction("GREATER_EQUAL", offset);
    case OpCode::LESS:
        return simpleInstruction("LESS", offset);
    case OpCode::LESS_EQUAL:
        return simpleInstruction("LESS_EQUAL", offset);
    case OpCode::NOT:
        return simpleInstruction("NOT", offset);
    case OpCode::XOR:
        return simpleInstruction("XOR", offset);
    case OpCode::AND:
        return simpleInstruction("AND", offset);
    case OpCode::OR:
        return simpleInstruction("OR", offset);
    case OpCode::SHL:
        return simpleInstruction("SHL", offset);
    case OpCode::SHR:
        return simpleInstruction("SHR", offset);



    case OpCode::BREAK:
        return simpleInstruction("BREAK", offset);
    case OpCode::CONTINUE:
        return simpleInstruction("CONTINUE", offset);

    case OpCode::DUP:
        return simpleInstruction("DUP", offset);
    case OpCode::EVAL_EQUAL:
        return simpleInstruction("EVAL_EQUAL", offset);

  
    case OpCode::GLOBAL_DEFINE:
        return constantInstruction("GLOBAL_DEFINE", offset);
    
    case OpCode::GLOBAL_SET:
        return simpleInstruction("GLOBAL_SET", offset);
    
    case OpCode::GLOBAL_ASSIGN:
        return constantInstruction("GLOBAL_ASSIGN", offset);
    case OpCode::GLOBAL_GET:
        return constantInstruction("GLOBAL_GET", offset);

    case OpCode::LOCAL_SET:
        return byteInstruction("LOCAL_SET", offset);

    case OpCode::LOCAL_GET:
        return byteInstruction("LOCAL_GET", offset);


    case OpCode::SWITCH:
        return simpleInstruction("SWITCH", offset);



    case OpCode::SAVE:
        return simpleInstruction("SAVE",  offset);

    case OpCode::RESTORE:
        return simpleInstruction("RESTORE",  offset);


    
    case OpCode::CASE:
        return jumpInstruction("CASE", 1, offset);
    case OpCode::SWITCH_DEFAULT:
        return jumpInstruction("DEFAULT", 1, offset);

    case OpCode::LOOP:
        return jumpInstruction("LOOP", -1, offset);


    case OpCode::BACK:
        return jumpInstruction("BACK", -1, offset);

    case OpCode::JUMP:
        return jumpInstruction("JUMP", 1, offset);



    case OpCode::JUMP_IF_FALSE:
        return jumpInstruction("JUMP_IF_FALSE", 1, offset);

    case OpCode::JUMP_IF_TRUE:
        return jumpInstruction("JUMP_IF_TRUE", 1, offset);

    case OpCode::INC:
          return simpleInstruction("INC", offset);
    case OpCode::DEC:
          return simpleInstruction("DEC", offset);
     case OpCode::RINC:
          return simpleInstruction("RINC", offset);
    case OpCode::RDEC:
          return simpleInstruction("RDEC", offset);

     case OpCode::OPADD:
          return simpleInstruction("OPADD", offset);
     case OpCode::OPSUB:
          return simpleInstruction("OPSUB", offset);
     case OpCode::OPMUL:
          return simpleInstruction("OPMUL", offset);
     case OpCode::OPDIV:
          return simpleInstruction("OPDIV", offset);

    case OpCode::CALL_NATIVE:
        return byteInstruction("CALL_NATIVE", offset);
    case OpCode::CALL_SCRIPT:
        return byteInstruction("CALL_SCRIPT", offset);



    default:
    {
        printf("Unknow instruction %d\n", instruction);
        return chunk->count;
        break;
    }
    }
    return offset;
}

u32 Compiler::byteInstruction(const char *name, u32 offset)
{
    u8 slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

u32 Compiler::jumpInstruction(const char *name, u32 sign, u32 offset)
{
    u16 jump = (u16)chunk->code[offset + 1] << 8;
    jump |= chunk->code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}






u32 Compiler::constantInstruction(const char *name, u32 offset)
{

    u8 constant = chunk->code[offset + 1];

    printf("%-16s %4d '", name, constant);
    Value value = constants[constant];

    debugValue(value);
    printf("'\n");

    return offset + 2;
}

u32 Compiler::simpleInstruction(const char *name, u32 offset)
{
    printf("%s\n", name);
    return offset + 1;
}
