#pragma once
#include "config.hpp"
#include "opcode.hpp"
#include "value.hpp"

#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <string>

class Interpreter;
class ProcessExec;
class Function;
class Code;

// ============================================================================
// RuntimeDebugger — Zero-overhead bytecode-patching debugger
//
// When no breakpoints are set, the VM runs at full speed (no per-opcode check).
// Breakpoints work by patching the original opcode byte with OP_BREAKPOINT.
// When the VM hits OP_BREAKPOINT, it restores the original opcode, opens an
// interactive prompt, and then re-dispatches the original instruction.
//
// Step mode temporarily patches the NEXT instruction with OP_BREAKPOINT
// (single-shot), so stepping also has zero overhead on the non-stepping path.
// ============================================================================

class RuntimeDebugger
{
public:
    // A single breakpoint record
    struct Breakpoint
    {
        Code   *chunk;          // Which chunk was patched
        size_t  offset;         // Byte offset in chunk->code
        uint8   originalOpcode; // The byte we replaced
        int     line;           // Source line
        bool    enabled;        // Can be temporarily disabled
    };

private:
    Interpreter *vm_;

    // All active breakpoints
    std::vector<Breakpoint> breakpoints_;

    // Step-mode state (one-shot breakpoint for step/next commands)
    bool    stepActive_;        // true = we have a one-shot step patch
    Code   *stepChunk_;         // chunk we patched for step
    size_t  stepOffset_;        // offset we patched
    uint8   stepOriginal_;      // original byte at that offset

    // Watch list (global variable names)
    std::map<std::string, Value> watches_;

public:
    explicit RuntimeDebugger(Interpreter *vm)
        : vm_(vm)
        , stepActive_(false)
        , stepChunk_(nullptr)
        , stepOffset_(0)
        , stepOriginal_(0)
    {}

    ~RuntimeDebugger()
    {
        removeAllBreakpoints();
        clearStep();
    }

    // ------------------------------------------------------------------
    // Breakpoint management — patches bytecode directly
    // ------------------------------------------------------------------

    // Set a breakpoint at a given source line.
    // Scans all compiled functions to find the first opcode on that line.
    // Returns true if at least one byte was patched.
    bool addBreakpoint(int line);

    // Remove all breakpoints on a given line. Restores original bytes.
    void removeBreakpoint(int line);

    // Remove every breakpoint. Restores all original bytes.
    void removeAllBreakpoints();

    // List breakpoints to stdout.
    void listBreakpoints() const;

    // ------------------------------------------------------------------
    // Step support — one-shot breakpoint on the NEXT instruction
    // ------------------------------------------------------------------

    // Patch the next instruction (at ip) with OP_BREAKPOINT.
    void armStep(ProcessExec *fiber);

    // Clear the one-shot step patch (restore byte).
    void clearStep();

    // ------------------------------------------------------------------
    // Called from the OP_BREAKPOINT handler
    // ------------------------------------------------------------------

    // Called by the VM when it hits OP_BREAKPOINT.
    // Returns the original opcode so the dispatch handler can jump to it
    // directly. ip is NOT rewound — it already points past the OP_BREAKPOINT
    // byte, so operand reads in the original handler work correctly.
    //
    // For regular breakpoints: OP_BREAKPOINT stays in the bytecode (persistent).
    // For one-shot steps: the patched byte is restored.
    uint8 onBreakpoint(ProcessExec *fiber, uint8 *ip);

    // ------------------------------------------------------------------
    // Inspection helpers
    // ------------------------------------------------------------------
    void printStack(ProcessExec *fiber) const;
    void printCallFrames(ProcessExec *fiber) const;
    void printLocals(ProcessExec *fiber) const;
    void printGlobals() const;
    void printProcesses() const;

    // Watch
    void watch(const char *varName);
    void updateWatches();

private:
    // Interactive prompt — blocks until user types continue/step/etc.
    void prompt(ProcessExec *fiber, int line);

    // Find the first bytecode offset that maps to `line` in `chunk`.
    // Returns (size_t)-1 if not found.
    static size_t findOffsetForLine(Code *chunk, int line);
};
