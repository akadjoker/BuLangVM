/**
 * @file RuntimeDebugger.cpp
 * @brief Zero-overhead bytecode-patching debugger implementation
 *
 * Breakpoints are implemented by patching opcode bytes in compiled bytecode
 * with OP_BREAKPOINT. When the VM encounters this opcode, it calls back into
 * the debugger which restores the original byte, opens an interactive prompt,
 * and returns so the VM can re-dispatch the original instruction.
 *
 * This approach has ZERO overhead when no breakpoints are set — the dispatch
 * loop is completely untouched.
 */

#include "RuntimeDebugger.hpp"
#include "interpreter.hpp"
#include "code.hpp"
#include "debug.hpp"

// ============================================================================
// Breakpoint management
// ============================================================================

size_t RuntimeDebugger::findOffsetForLine(Code *chunk, int line)
{
    if (!chunk || !chunk->code || !chunk->lines)
        return (size_t)-1;

    for (size_t i = 0; i < chunk->count; i++)
    {
        if (chunk->lines[i] == line)
            return i;
    }
    return (size_t)-1;
}

bool RuntimeDebugger::addBreakpoint(int line)
{
    bool patched = false;

    // Scan all script functions
    for (size_t fi = 0; fi < vm_->functions.size(); fi++)
    {
        Function *func = vm_->functions[fi];
        if (!func || !func->chunk) continue;

        size_t offset = findOffsetForLine(func->chunk, line);
        if (offset == (size_t)-1) continue;

        // Don't double-patch
        uint8 current = func->chunk->code[offset];
        if (current == OP_BREAKPOINT) continue;

        Breakpoint bp;
        bp.chunk          = func->chunk;
        bp.offset         = offset;
        bp.originalOpcode = current;
        bp.line           = line;
        bp.enabled        = true;
        breakpoints_.push_back(bp);

        func->chunk->code[offset] = OP_BREAKPOINT;
        patched = true;
    }

    // Scan class methods
    for (size_t ci = 0; ci < vm_->classes.size(); ci++)
    {
        ClassDef *cls = vm_->classes[ci];
        if (!cls) continue;

        // Constructor
        if (cls->constructor && cls->constructor->chunk)
        {
            size_t offset = findOffsetForLine(cls->constructor->chunk, line);
            if (offset != (size_t)-1 && cls->constructor->chunk->code[offset] != OP_BREAKPOINT)
            {
                Breakpoint bp;
                bp.chunk          = cls->constructor->chunk;
                bp.offset         = offset;
                bp.originalOpcode = cls->constructor->chunk->code[offset];
                bp.line           = line;
                bp.enabled        = true;
                breakpoints_.push_back(bp);
                cls->constructor->chunk->code[offset] = OP_BREAKPOINT;
                patched = true;
            }
        }

        // Methods
        for (size_t mi = 0; mi < cls->methods.count; mi++)
        {
            Function *method = cls->methods.entries[mi].value;
            if (!method || !method->chunk) continue;

            size_t offset = findOffsetForLine(method->chunk, line);
            if (offset == (size_t)-1) continue;
            if (method->chunk->code[offset] == OP_BREAKPOINT) continue;

            Breakpoint bp;
            bp.chunk          = method->chunk;
            bp.offset         = offset;
            bp.originalOpcode = method->chunk->code[offset];
            bp.line           = line;
            bp.enabled        = true;
            breakpoints_.push_back(bp);
            method->chunk->code[offset] = OP_BREAKPOINT;
            patched = true;
        }
    }

    // Scan process definitions
    for (size_t pi = 0; pi < vm_->processes.size(); pi++)
    {
        ProcessDef *proc = vm_->processes[pi];
        if (!proc || proc->frameCount == 0) continue;

        // Process code is in the function of its first frame
        Function *func = proc->frames[0].func;
        if (!func || !func->chunk) continue;

        size_t offset = findOffsetForLine(func->chunk, line);
        if (offset == (size_t)-1) continue;
        if (func->chunk->code[offset] == OP_BREAKPOINT) continue;

        Breakpoint bp;
        bp.chunk          = func->chunk;
        bp.offset         = offset;
        bp.originalOpcode = func->chunk->code[offset];
        bp.line           = line;
        bp.enabled        = true;
        breakpoints_.push_back(bp);
        func->chunk->code[offset] = OP_BREAKPOINT;
        patched = true;
    }

    if (patched)
        printf("Breakpoint set at line %d\n", line);
    else
        printf("No code found at line %d\n", line);

    return patched;
}

void RuntimeDebugger::removeBreakpoint(int line)
{
    for (auto it = breakpoints_.begin(); it != breakpoints_.end(); )
    {
        if (it->line == line)
        {
            // Restore original opcode
            it->chunk->code[it->offset] = it->originalOpcode;
            it = breakpoints_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void RuntimeDebugger::removeAllBreakpoints()
{
    for (auto &bp : breakpoints_)
    {
        if (bp.chunk && bp.chunk->code)
            bp.chunk->code[bp.offset] = bp.originalOpcode;
    }
    breakpoints_.clear();
}

void RuntimeDebugger::listBreakpoints() const
{
    if (breakpoints_.empty())
    {
        printf("No breakpoints set.\n");
        return;
    }

    printf("\n=== BREAKPOINTS ===\n");
    for (size_t i = 0; i < breakpoints_.size(); i++)
    {
        const Breakpoint &bp = breakpoints_[i];
        printf("  #%zu  line %d  [%s]\n", i, bp.line,
               bp.enabled ? "enabled" : "disabled");
    }
    printf("===================\n");
}

// ============================================================================
// Step support
// ============================================================================

void RuntimeDebugger::armStep(ProcessExec *fiber)
{
    clearStep(); // Remove any previous step

    if (!fiber || fiber->frameCount == 0) return;

    CallFrame *frame = &fiber->frames[fiber->frameCount - 1];
    Code *chunk = frame->func->chunk;
    size_t offset = frame->ip - chunk->code;

    if (offset >= chunk->count) return;

    // Find the first opcode on a DIFFERENT source line.
    // Line changes always align with opcode boundaries (the compiler
    // emits all bytes of a multi-byte instruction under the same line),
    // so this is always safe to patch.
    int currentLine = chunk->lines[offset];
    size_t targetOffset = offset;
    for (size_t i = offset + 1; i < chunk->count; i++)
    {
        if (chunk->lines[i] != currentLine)
        {
            targetOffset = i;
            break;
        }
    }

    // No different line found — can't step
    if (targetOffset == offset) return;

    // Don't patch if already a breakpoint
    if (chunk->code[targetOffset] == OP_BREAKPOINT) return;

    stepChunk_    = chunk;
    stepOffset_   = targetOffset;
    stepOriginal_ = chunk->code[targetOffset];
    stepActive_   = true;

    chunk->code[targetOffset] = OP_BREAKPOINT;
}

void RuntimeDebugger::clearStep()
{
    if (stepActive_ && stepChunk_ && stepChunk_->code)
    {
        stepChunk_->code[stepOffset_] = stepOriginal_;
    }
    stepActive_ = false;
    stepChunk_  = nullptr;
    stepOffset_ = 0;
    stepOriginal_ = 0;
}

// ============================================================================
// OP_BREAKPOINT handler
// ============================================================================

uint8 RuntimeDebugger::onBreakpoint(ProcessExec *fiber, uint8 *ip)
{
    // ip points PAST the OP_BREAKPOINT byte. The patched location is ip-1.
    CallFrame *frame = &fiber->frames[fiber->frameCount - 1];
    Code *chunk = frame->func->chunk;
    size_t patchOffset = (ip - 1) - chunk->code;

    // Determine source line
    int line = chunk->lines[patchOffset];

    // Case 1: Step hit (one-shot — restore the patched byte)
    if (stepActive_ && stepChunk_ == chunk && stepOffset_ == patchOffset)
    {
        uint8 orig = stepOriginal_;

        // Restore the step-patched byte (one-shot, remove the step)
        chunk->code[patchOffset] = orig;
        stepActive_ = false;
        stepChunk_  = nullptr;

        // Open prompt so user can inspect/continue
        prompt(fiber, line);
        return orig;
    }

    // Case 2: Regular breakpoint (persistent — OP_BREAKPOINT stays in bytecode)
    for (size_t i = 0; i < breakpoints_.size(); i++)
    {
        Breakpoint &bp = breakpoints_[i];
        if (bp.chunk == chunk && bp.offset == patchOffset)
        {
            // Open prompt
            prompt(fiber, line);

            // Return the original opcode for the dispatch handler to execute.
            // OP_BREAKPOINT stays in the bytecode so it fires again next time.
            return bp.originalOpcode;
        }
    }

    // Unknown — shouldn't happen. Print warning and return a harmless opcode.
    printf("[debugger] unexpected OP_BREAKPOINT at offset %zu line %d\n",
           patchOffset, line);
    // Restore the byte to OP_NIL so we don't loop forever
    chunk->code[patchOffset] = OP_NIL;
    return OP_NIL;
}

// ============================================================================
// Inspection
// ============================================================================

void RuntimeDebugger::printStack(ProcessExec *fiber) const
{
    printf("\n=== STACK (top to bottom) ===\n");

    if (fiber->stackTop == fiber->stack)
    {
        printf("  (empty)\n");
        return;
    }

    int index = 0;
    for (Value *slot = fiber->stackTop - 1; slot >= fiber->stack; slot--)
    {
        printf("  [%2d] ", index++);
        printValue(*slot);
        printf("\n");
    }
    printf("=============================\n");
}

void RuntimeDebugger::printCallFrames(ProcessExec *fiber) const
{
    printf("\n=== CALL FRAMES ===\n");

    for (int i = fiber->frameCount - 1; i >= 0; i--)
    {
        CallFrame *frame = &fiber->frames[i];
        Function *func = frame->func;

        size_t instruction = frame->ip - func->chunk->code;
        if (instruction > 0) instruction--;
        int line = func->chunk->lines[instruction];

        printf("  [%d] %s() at line %d\n",
               i,
               func->name ? func->name->chars() : "<script>",
               line);
    }
    printf("===================\n");
}

void RuntimeDebugger::printLocals(ProcessExec *fiber) const
{
    printf("\n=== LOCAL VARIABLES ===\n");

    if (fiber->frameCount == 0)
    {
        printf("  (no active frame)\n");
        return;
    }

    CallFrame *frame = &fiber->frames[fiber->frameCount - 1];
    Value *slots = frame->slots;
    int slotCount = (int)(fiber->stackTop - slots);

    for (int i = 0; i < slotCount; i++)
    {
        printf("  local[%d] = ", i);
        printValue(slots[i]);
        printf("\n");
    }
    printf("=======================\n");
}

void RuntimeDebugger::printGlobals() const
{
    printf("\n=== GLOBAL VARIABLES ===\n");

    for (size_t i = 0; i < vm_->globals.capacity; i++)
    {
        if (vm_->globals.entries[i].state != 1) // 1 = FILLED
            continue;
        String *name = vm_->globals.entries[i].key;
        const Value &value = vm_->globals.entries[i].value;
        printf("  %s = ", name->chars());
        printValue(value);
        printf("\n");
    }
    printf("========================\n");
}

void RuntimeDebugger::printProcesses() const
{
    printf("\n=== PROCESSES ===\n");
    printf("Alive: %zu\n", vm_->aliveProcesses.size());

    for (size_t i = 0; i < vm_->aliveProcesses.size(); i++)
    {
        Process *proc = vm_->aliveProcesses[i];
        if (!proc) continue;

        printf("  [%u] %s (state=%d)\n",
               proc->id,
               proc->name ? proc->name->chars() : "<unnamed>",
               (int)proc->state);
    }
    printf("=================\n");
}

// ============================================================================
// Watch support
// ============================================================================

void RuntimeDebugger::watch(const char *varName)
{
    Value nil;
    nil.type = ValueType::NIL;
    watches_[varName] = nil;
    printf("Watching variable '%s'\n", varName);
}

void RuntimeDebugger::updateWatches()
{
    for (auto it = watches_.begin(); it != watches_.end(); ++it)
    {
        const std::string &name = it->first;
        Value &oldValue = it->second;

        String *nameStr = vm_->stringPool.create(name.c_str());
        Value newValue;
        if (vm_->globals.get(nameStr, &newValue) && !valuesEqual(oldValue, newValue))
        {
            printf("WATCH: '%s' changed from ", name.c_str());
            printValue(oldValue);
            printf(" to ");
            printValue(newValue);
            printf("\n");

            it->second = newValue;
        }
    }
}

// ============================================================================
// Interactive prompt
// ============================================================================

void RuntimeDebugger::prompt(ProcessExec *fiber, int line)
{
    printf("\n>>> Breakpoint at line %d\n", line);

    // Show current location
    if (fiber->frameCount > 0)
    {
        CallFrame *frame = &fiber->frames[fiber->frameCount - 1];
        const char *fname = frame->func->name
            ? frame->func->name->chars() : "<script>";
        printf("    in %s()\n", fname);
    }

    // Update watches
    updateWatches();

    while (true)
    {
        printf("(debug) ");
        fflush(stdout);

        char input[256];
        if (!fgets(input, sizeof(input), stdin))
            break;

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (input[0] == '\0') continue;

        if (strcmp(input, "c") == 0 || strcmp(input, "continue") == 0)
        {
            break;
        }
        else if (strcmp(input, "s") == 0 || strcmp(input, "step") == 0)
        {
            armStep(fiber);
            break;
        }
        else if (strcmp(input, "n") == 0 || strcmp(input, "next") == 0)
        {
            // For now, 'next' behaves like 'step' (step to next line)
            armStep(fiber);
            break;
        }
        else if (strcmp(input, "stack") == 0 || strcmp(input, "st") == 0)
        {
            printStack(fiber);
        }
        else if (strcmp(input, "frames") == 0 || strcmp(input, "bt") == 0)
        {
            printCallFrames(fiber);
        }
        else if (strcmp(input, "locals") == 0 || strcmp(input, "l") == 0)
        {
            printLocals(fiber);
        }
        else if (strcmp(input, "globals") == 0 || strcmp(input, "g") == 0)
        {
            printGlobals();
        }
        else if (strcmp(input, "processes") == 0 || strcmp(input, "ps") == 0)
        {
            printProcesses();
        }
        else if (strcmp(input, "breakpoints") == 0 || strcmp(input, "bl") == 0)
        {
            listBreakpoints();
        }
        else if (strncmp(input, "b ", 2) == 0)
        {
            int bpLine = atoi(input + 2);
            if (bpLine > 0)
                addBreakpoint(bpLine);
            else
                printf("Usage: b <line_number>\n");
        }
        else if (strncmp(input, "d ", 2) == 0)
        {
            int bpLine = atoi(input + 2);
            if (bpLine > 0)
            {
                removeBreakpoint(bpLine);
                printf("Breakpoint at line %d removed.\n", bpLine);
            }
            else
                printf("Usage: d <line_number>\n");
        }
        else if (strncmp(input, "watch ", 6) == 0)
        {
            watch(input + 6);
        }
        else if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0)
        {
            printf("Detaching debugger.\n");
            removeAllBreakpoints();
            clearStep();
            break;
        }
        else if (strcmp(input, "help") == 0 || strcmp(input, "h") == 0)
        {
            printf("Commands:\n");
            printf("  c, continue    - Continue execution\n");
            printf("  s, step        - Step to next line\n");
            printf("  n, next        - Step to next line\n");
            printf("  st, stack      - Print stack\n");
            printf("  bt, frames     - Print call frames (backtrace)\n");
            printf("  l, locals      - Print local variables\n");
            printf("  g, globals     - Print global variables\n");
            printf("  ps, processes  - Print alive processes\n");
            printf("  bl, breakpoints - List breakpoints\n");
            printf("  b <line>       - Add breakpoint at line\n");
            printf("  d <line>       - Delete breakpoint at line\n");
            printf("  watch <var>    - Watch global variable\n");
            printf("  q, quit        - Detach debugger\n");
            printf("  h, help        - Show this help\n");
        }
        else
        {
            printf("Unknown command '%s'. Type 'help' for commands.\n", input);
        }
    }
}
