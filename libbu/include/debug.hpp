#pragma once
#include "config.hpp"

struct Function;
class Code;

class Debug
{
public:
    // Para uso do Compiler - setar nomes globais antes de dump
    static void setGlobalNames(const char** names, int count);
    static void clearGlobalNames();
    static void setOutput(FILE *output);

    // Disassemble um chunk inteiro
    static void disassembleChunk(const Code &chunk, const char *name);

    static void dumpFunction(const Function *func);

    // Disassemble a single instruction
    static size_t disassembleInstruction(const Code &chunk, size_t offset);

private:
    // Helpers by instruction type
    static size_t simpleInstruction(const char *name, size_t offset);

    static size_t constantInstruction(
        const char *name,
        const Code &chunk,
        size_t offset);

    // For globals (prints variable name if string)
    static size_t constantNameInstruction(
        const char *name,
        const Code &chunk,
        size_t offset);

    // For globals with direct array index (OPTIMIZATION)
    static size_t globalIndexInstruction(
        const char *name,
        const Code &chunk,
        size_t offset);

    static size_t byteInstruction(
        const char *name,
        const Code &chunk,
        size_t offset);

    static size_t shortInstruction(
        const char *name,
        const Code &chunk,
        size_t offset);

    static size_t jumpInstruction(
        const char *name,
        int sign,
        const Code &chunk,
        size_t offset);

 
};
