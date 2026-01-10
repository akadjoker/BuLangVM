#pragma once
#include "config.hpp"

enum Opcode : uint8
{
    // Literals (0-3)
    OP_CONSTANT = 0,
    OP_NIL = 1,
    OP_TRUE = 2,
    OP_FALSE = 3,

    // Stack (4-7)
    OP_POP = 4,
    OP_HALT = 5,
    OP_NOT = 6,
    OP_DUP = 7,

    // Arithmetic (8-13)
    OP_ADD = 8,
    OP_SUBTRACT = 9,
    OP_MULTIPLY = 10,
    OP_DIVIDE = 11,
    OP_NEGATE = 12,
    OP_MODULO = 13,

    // Bitwise (14-19)
    OP_BITWISE_AND = 14,
    OP_BITWISE_OR = 15,
    OP_BITWISE_XOR = 16,
    OP_BITWISE_NOT = 17,
    OP_SHIFT_LEFT = 18,
    OP_SHIFT_RIGHT = 19,

    // Comparisons (20-25)
    OP_EQUAL = 20,
    OP_NOT_EQUAL = 21,
    OP_GREATER = 22,
    OP_GREATER_EQUAL = 23,
    OP_LESS = 24,
    OP_LESS_EQUAL = 25,

    // Variables (26-32)
    OP_GET_LOCAL = 26,
    OP_SET_LOCAL = 27,
    OP_GET_GLOBAL = 28,
    OP_SET_GLOBAL = 29,
    OP_DEFINE_GLOBAL = 30,
    OP_GET_PRIVATE = 31,
    OP_SET_PRIVATE = 32,

    // Control flow (33-37)
    OP_JUMP = 33,
    OP_JUMP_IF_FALSE = 34,
    OP_LOOP = 35,
    OP_GOSUB = 36,
    OP_RETURN_SUB = 37,

    // Functions (38-43)
    OP_CALL = 38,
    OP_RETURN = 39,
    OP_SPAWN = 40,
    OP_YIELD = 41,
    OP_FRAME = 42,
    OP_EXIT = 43,

    // Collections (44-44)
    OP_DEFINE_ARRAY = 44,
    OP_DEFINE_MAP = 45,

    // Properties (46-49)
    OP_GET_PROPERTY = 46,
    OP_SET_PROPERTY = 47,
    OP_GET_INDEX = 48,
    OP_SET_INDEX = 49,

    // Methods (50-51)
    OP_INVOKE = 50,
    OP_SUPER_INVOKE = 51,

    // I/O (52)
    OP_PRINT = 52,
    OP_FUNC_LEN = 53,

    // Foreach
    OP_ITER_NEXT = 54,
    OP_ITER_VALUE = 55,
    OP_COPY2 = 56,
    OP_SWAP = 57,
    OP_DISCARD = 58,
    OP_TRY = 59,           // Setup try handler
    OP_POP_TRY = 60,       // Remove try handler (normal exit)
    OP_THROW = 61,         // Lança exceção
    OP_ENTER_CATCH = 62,   // Entra no catch
    OP_ENTER_FINALLY = 63, // Entra no finally
    OP_EXIT_FINALLY = 64,  // Sai do finally (re-throw se necessário)
    // features
    // --- MATH UNARY (1 Argumento) ---
    OP_SIN = 65,
    OP_COS = 66,
    OP_TAN = 67,
    OP_ASIN = 68,
    OP_ACOS = 69,
    OP_ATAN = 70,
    OP_SQRT = 71,
    OP_ABS = 72,
    OP_LOG = 73,
    OP_FLOOR = 74,
    OP_CEIL = 75,
    OP_DEG = 76,
    OP_RAD = 77,
    OP_EXP =78,

    // --- MATH BINARY (2 Argumentos) ---
    OP_ATAN2 = 79,
    OP_POW = 80,

    OP_CLOCK = 81,
    //new type buffer
    OP_NEW_BUFFER = 82

};