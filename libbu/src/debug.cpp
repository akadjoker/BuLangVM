#include "debug.hpp"
#include "code.hpp"
#include "interpreter.hpp"
#include "opcode.hpp"
#include <cstdio>

void Debug::disassembleChunk(const Code &chunk, const char *name)
{
  printf("== %s ==\n", name);

  for (size_t offset = 0; offset < chunk.count;)
  {
    offset = disassembleInstruction(chunk, offset);
  }
}

static bool hasBytes(const Code &chunk, size_t offset, size_t n)
{
  return offset + n < chunk.count; // offset + n is last index read
}

size_t Debug::disassembleInstruction(const Code &chunk, size_t offset)
{
  printf("%04zu ", offset);

  if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1])
    printf("   | ");
  else
    printf("%4d ", chunk.lines[offset]);

  if (offset >= chunk.count)
  {
    printf("<<out of bounds>>\n");
    return offset + 1;
  }

  uint8 instruction = chunk.code[offset];

  switch (instruction)
  {
    // -------- Literals --------
  case OP_CONSTANT:
  {
    uint8_t constant = chunk.code[offset + 1];  
    printf("%-16s %4d '", "OP_CONSTANT", constant);
    printValue(chunk.constants[constant]);
    printf("'\n");
    return offset + 2; 
  }
  case OP_NIL:
    return simpleInstruction("OP_NIL", offset);
  case OP_TRUE:
    return simpleInstruction("OP_TRUE", offset);
  case OP_FALSE:
    return simpleInstruction("OP_FALSE", offset);

  // Bitwise (6 cases)
  case OP_BITWISE_AND:
    return simpleInstruction("OP_BITWISE_AND", offset);
  case OP_BITWISE_OR:
    return simpleInstruction("OP_BITWISE_OR", offset);
  case OP_BITWISE_XOR:
    return simpleInstruction("OP_BITWISE_XOR", offset);
  case OP_BITWISE_NOT:
    return simpleInstruction("OP_BITWISE_NOT", offset);
  case OP_SHIFT_LEFT:
    return simpleInstruction("OP_SHIFT_LEFT", offset);
  case OP_SHIFT_RIGHT:
    return simpleInstruction("OP_SHIFT_RIGHT", offset);

  // Stack
  case OP_DUP:
    return simpleInstruction("OP_DUP", offset);

  // Process control
  case OP_SPAWN:
    return byteInstruction("OP_SPAWN", chunk, offset);
  case OP_EXIT:
    return simpleInstruction("OP_EXIT", offset);

  // -------- Stack --------
  case OP_POP:
    return simpleInstruction("OP_POP", offset);
  case OP_HALT:
    return simpleInstruction("OP_HALT", offset);
  case OP_NOT:
    return simpleInstruction("OP_NOT", offset);

  // -------- Arithmetic --------
  case OP_ADD:
    return simpleInstruction("OP_ADD", offset);
  case OP_SUBTRACT:
    return simpleInstruction("OP_SUBTRACT", offset);
  case OP_MULTIPLY:
    return simpleInstruction("OP_MULTIPLY", offset);
  case OP_DIVIDE:
    return simpleInstruction("OP_DIVIDE", offset);
  case OP_NEGATE:
    return simpleInstruction("OP_NEGATE", offset);
  case OP_MODULO:
    return simpleInstruction("OP_MODULO", offset);

  case OP_GET_PROPERTY:
    return constantInstruction("OP_GET_PROPERTY", chunk, offset);
  case OP_SET_PROPERTY:
    return constantInstruction("OP_SET_PROPERTY", chunk, offset);

  // -------- Comparisons --------
  case OP_EQUAL:
    return simpleInstruction("OP_EQUAL", offset);
  case OP_NOT_EQUAL:
    return simpleInstruction("OP_NOT_EQUAL", offset);
  case OP_GREATER:
    return simpleInstruction("OP_GREATER", offset);
  case OP_GREATER_EQUAL:
    return simpleInstruction("OP_GREATER_EQUAL", offset);
  case OP_LESS:
    return simpleInstruction("OP_LESS", offset);
  case OP_LESS_EQUAL:
    return simpleInstruction("OP_LESS_EQUAL", offset);

  // -------- Variables --------
  case OP_GET_LOCAL:
    return byteInstruction("OP_GET_LOCAL", chunk, offset);
  case OP_SET_LOCAL:
    return byteInstruction("OP_SET_LOCAL", chunk, offset);
    

  // Globals: operand é índice para constants (normalmente String)
  case OP_GET_GLOBAL:
    return constantNameInstruction("OP_GET_GLOBAL", chunk, offset);
  case OP_SET_GLOBAL:
    return constantNameInstruction("OP_SET_GLOBAL", chunk, offset);
  case OP_DEFINE_GLOBAL:
    return constantNameInstruction("OP_DEFINE_GLOBAL", chunk, offset);

  case OP_GET_PRIVATE:
    return byteInstruction("OP_GET_PRIVATE", chunk, offset);
  case OP_SET_PRIVATE:
    return byteInstruction("OP_SET_PRIVATE", chunk, offset);

  // -------- Control flow --------
  case OP_JUMP:
    return jumpInstruction("OP_JUMP", +1, chunk, offset);
  case OP_JUMP_IF_FALSE:
    return jumpInstruction("OP_JUMP_IF_FALSE", +1, chunk, offset);
  case OP_LOOP:
    return jumpInstruction("OP_LOOP", -1, chunk, offset);

  // -------- Functions --------
  case OP_CALL:
    return byteInstruction("OP_CALL", chunk, offset);

  // case OP_CALL_NATIVE:
  // {
  //   // operands: nameIdx (constant) + argCount
  //   if (!hasBytes(chunk, offset, 2))
  //   {
  //     printf("OP_CALL_NATIVE <truncated>\n");
  //     return chunk.count;
  //   }
  //   uint8 nameIdx = chunk.code[offset + 1];
  //   uint8 argCount = chunk.code[offset + 2];

  //   Value c = chunk.constants[nameIdx];
  //   const char *nm = (c.isString() ? c.asString()->chars() : "<non-string>");

  //   printf("%-16s %4u '%s' (%u args)\n", "OP_CALL_NATIVE", (unsigned)nameIdx,
  //          nm, (unsigned)argCount);

  //   return offset + 3;
  // }
  case OP_INVOKE:
  {
    if (!hasBytes(chunk, offset, 2))
    {
      printf("OP_INVOKE <truncated>\n");
      return chunk.count;
    }

    uint8_t nameIdx = chunk.code[offset + 1];
    uint8_t argCount = chunk.code[offset + 2];

    Value c = chunk.constants[nameIdx];
    const char *nm = (c.isString() ? c.asString()->chars() : "<non-string>");

    printf("%-16s %4u '%s' (%u args)\n", "OP_INVOKE", (unsigned)nameIdx, nm,
           (unsigned)argCount);

    return offset + 3;
  }


  case OP_DEFINE_ARRAY:
    return byteInstruction("OP_DEFINE_ARRAY", chunk, offset);

  case OP_DEFINE_MAP:
    return simpleInstruction("OP_DEFINE_MAP", offset);

  // case OP_DEFINE_STRUCT:
  //   return byteInstruction("OP_DEFINE_STRUCT", chunk, offset);

  case OP_GET_INDEX:
    return simpleInstruction("OP_GET_INDEX", offset);

  case OP_SET_INDEX:
    return simpleInstruction("OP_SET_INDEX", offset);

case OP_FOREACH_START:
    return simpleInstruction("OP_FOREACH_START", offset);
    
case OP_FOREACH_NEXT:
    return simpleInstruction("OP_FOREACH_NEXT", offset);
    
case OP_FOREACH_CHECK:
    return simpleInstruction("OP_FOREACH_CHECK", offset); 
    
 

  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  // case OP_RETURN_NIL:
  //   return simpleInstruction("OP_RETURN_NIL", offset);

  // -------- Scheduling --------
  case OP_YIELD:
    return simpleInstruction("OP_YIELD", offset);
  case OP_FRAME:
    return simpleInstruction("OP_FRAME", offset);
  case OP_FUNC_LEN:
    return simpleInstruction("OP_LEN", offset);
  // -------- I/O --------
  case OP_PRINT:
    return simpleInstruction("OP_PRINT", offset);

  case OP_SUPER_INVOKE:
  {
    if (!hasBytes(chunk, offset, 2))
    {
      printf("OP_INVOKE <truncated>\n");
      return chunk.count;
    }

    uint8_t nameIdx = chunk.code[offset + 1];
    uint8_t argCount = chunk.code[offset + 2];

    Value c = chunk.constants[nameIdx];
    const char *nm = (c.isString() ? c.asString()->chars() : "<non-string>");

    printf("%-16s %4u '%s' (%u args)\n", "OP_SUPER_INVOKE", (unsigned)nameIdx,
           nm, (unsigned)argCount);

    return offset + 3;
  }

    //         // Arrays (futuros)
    // case OP_NEW_ARRAY:
    //     return byteInstruction("OP_NEW_ARRAY", chunk, offset);
    // case OP_GET_INDEX:
    //     return simpleInstruction("OP_GET_INDEX", offset);
    // case OP_SET_INDEX:
    //     return simpleInstruction("OP_SET_INDEX", offset);

    // // Structs (futuros)
    // case OP_NEW_STRUCT:
    //     return byteInstruction("OP_NEW_STRUCT", chunk, offset);

    // // Fibers
    // case OP_CREATE_FIBER:
    //     return simpleInstruction("OP_CREATE_FIBER", offset);
  default:
    printf("Unknown opcode %u\n", (unsigned)instruction);
    return offset + 1;
  }
}

size_t Debug::simpleInstruction(const char *name, size_t offset)
{
  printf("%s\n", name);
  return offset + 1;
}

size_t Debug::constantInstruction(const char *name, const Code &chunk,
                                  size_t offset)
{
  if (offset + 1 >= chunk.count)
  {
    printf("%s <truncated>\n", name);
    return chunk.count;
  }

  uint8 constantIdx = chunk.code[offset + 1];
  printf("%-16s %4u '", name, (unsigned)constantIdx);
  printValue(chunk.constants[constantIdx]);
  printf("'\n");
  return offset + 2;
}

// Para globals: tenta imprimir o nome (string) além do índice.
size_t Debug::constantNameInstruction(const char *name, const Code &chunk,
                                      size_t offset)
{
  if (offset + 1 >= chunk.count)
  {
    printf("%s <truncated>\n", name);
    return chunk.count;
  }

  uint8 constantIdx = chunk.code[offset + 1];
  Value c = chunk.constants[constantIdx];
  const char *nm = (c.isString() ? c.asString()->chars() : "<non-string>");

  printf("%-16s %4u '%s'\n", name, (unsigned)constantIdx, nm);
  return offset + 2;
}

size_t Debug::byteInstruction(const char *name, const Code &chunk,
                              size_t offset)
{
  if (offset + 1 >= chunk.count)
  {
    printf("%s <truncated>\n", name);
    return chunk.count;
  }

  uint8 operand = chunk.code[offset + 1];
  printf("%-16s %4u\n", name, (unsigned)operand);
  return offset + 2;
}

size_t Debug::jumpInstruction(const char *name, int sign, const Code &chunk,
                              size_t offset)
{
  if (offset + 2 >= chunk.count)
  {
    printf("%s <truncated>\n", name);
    return chunk.count;
  }

  uint16 jump =
      (uint16)(chunk.code[offset + 1] << 8) | (uint16)chunk.code[offset + 2];
  long long target = (long long)offset + 3 + (long long)sign * (long long)jump;

  printf("%-16s %4zu -> %lld\n", name, offset, target);
  return offset + 3;
}

void Debug::dumpFunction(const Function *func)
{
  const char *name = (func->name && func->name->length() > 0)
                         ? func->name->chars()
                         : "<script>";

  printf("\n==============================\n");
  printf("Function %s\n", name);
  printf("arity: %d\n", func->arity);
  printf("hasReturn: %s\n", func->hasReturn ? "yes" : "no");
  printf("==============================\n\n");

  // ---- CONSTANTS ----
  printf("Constants (%zu):\n", func->chunk->constants.size());
  for (size_t i = 0; i < func->chunk->constants.size(); i++)
  {
    printf("%4zu = ", i);
    printValue(func->chunk->constants[i]);
    printf("\n");
  }

  printf("\n");

  // ---- BYTECODE ----
  disassembleChunk(*func->chunk, name);
}
