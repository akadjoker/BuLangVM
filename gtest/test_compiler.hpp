#pragma once

#include <cassert>
#include <cstring>
#include "compiler.hpp"
#include "token.hpp"

class CompilerTests
{
public:
    static void runAllTests()
    {
        testLocalStruct();
        testUpvalueInfo();
        testLoopContext();
        testLabelStruct();
        testGotoJump();
        testCompilerOptions();
        testParseRule();
        testPrecedenceEnum();
    }

private:
    static void testLocalStruct()
    {
        Local local;
        assert(local.depth == -1);
        assert(local.usedInitLocal == false);
        assert(local.isCaptured == false);

        local.name = "test";
        local.depth = 0;
        assert(local.equals("test"));
        assert(local.equals("test", 4));
        assert(!local.equals("other"));
    }

    static void testUpvalueInfo()
    {
        UpvalueInfo upval;
        upval.index = 5;
        upval.isLocal = true;
        assert(upval.index == 5);
        assert(upval.isLocal == true);
    }

    static void testLoopContext()
    {
        LoopContext loop;
        assert(loop.loopStart == 0);
        assert(loop.breakCount == 0);
        assert(loop.scopeDepth == 0);
        assert(loop.isForeach == false);

        assert(loop.addBreak(10));
        assert(loop.breakCount == 1);
        assert(loop.breakJumps[0] == 10);

        for (int i = 1; i < MAX_BREAKS_PER_LOOP; i++)
        {
            assert(loop.addBreak(i * 10));
        }
        assert(loop.addBreak(9999) == false);
    }

    static void testLabelStruct()
    {
        Label label;
        label.name = "start";
        label.offset = 42;
        assert(label.name == "start");
        assert(label.offset == 42);
    }

    static void testGotoJump()
    {
        GotoJump jump;
        jump.target = "loop_end";
        jump.jumpOffset = 100;
        assert(jump.target == "loop_end");
        assert(jump.jumpOffset == 100);
    }

    static void testCompilerOptions()
    {
        CompilerOptions opts;
        assert(opts.strictMode == true);
        assert(opts.allowUnsafeCode == false);
        assert(opts.maxSourceSize == 1024 * 1024);
        assert(opts.maxTokens == 100000);
        assert(opts.maxFunctions == 10000);
        assert(opts.maxConstants == 65535);
        assert(opts.validateUnicode == true);
        assert(opts.checkIntegerOverflow == true);
    }

    static void testParseRule()
    {
        ParseRule rule;
        rule.prefix = nullptr;
        rule.infix = nullptr;
        rule.prec = PREC_NONE;
        assert(rule.prec == PREC_NONE);
    }

    static void testPrecedenceEnum()
    {
        assert(PREC_NONE < PREC_ASSIGNMENT);
        assert(PREC_ASSIGNMENT < PREC_OR);
        assert(PREC_OR < PREC_AND);
        assert(PREC_AND < PREC_BITWISE_OR);
        assert(PREC_BITWISE_OR < PREC_BITWISE_XOR);
        assert(PREC_BITWISE_XOR < PREC_BITWISE_AND);
        assert(PREC_BITWISE_AND < PREC_EQUALITY);
        assert(PREC_EQUALITY < PREC_COMPARISON);
        assert(PREC_COMPARISON < PREC_SHIFT);
        assert(PREC_SHIFT < PREC_TERM);
        assert(PREC_TERM < PREC_FACTOR);
        assert(PREC_FACTOR < PREC_UNARY);
        assert(PREC_UNARY < PREC_CALL);
        assert(PREC_CALL < PREC_PRIMARY);
    }
};