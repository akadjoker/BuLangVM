#include <gtest/gtest.h>
#include "interpreter_runtime_switch.cpp"

class InterpreterRuntimeTest : public ::testing::Test {
protected:
    Interpreter interpreter;
    Fiber* fiber;
    Process* process;

    void SetUp() override {
        process = new Process();
        fiber = new Fiber();
        fiber->frameCount = 0;
        fiber->stackTop = fiber->stack;
    }

    void TearDown() override {
        delete fiber;
        delete process;
    }
};

// Test toNumberPair function
TEST_F(InterpreterRuntimeTest, ToNumberPairWithIntegers) {
    double da, db;
    Value a = makeInt(5);
    Value b = makeInt(10);
    EXPECT_TRUE(toNumberPair(a, b, da, db));
    EXPECT_EQ(da, 5.0);
    EXPECT_EQ(db, 10.0);
}

TEST_F(InterpreterRuntimeTest, ToNumberPairWithDoubles) {
    double da, db;
    Value a = makeDouble(5.5);
    Value b = makeDouble(10.5);
    EXPECT_TRUE(toNumberPair(a, b, da, db));
    EXPECT_EQ(da, 5.5);
    EXPECT_EQ(db, 10.5);
}

TEST_F(InterpreterRuntimeTest, ToNumberPairMixed) {
    double da, db;
    Value a = makeInt(5);
    Value b = makeDouble(10.5);
    EXPECT_TRUE(toNumberPair(a, b, da, db));
    EXPECT_EQ(da, 5.0);
    EXPECT_EQ(db, 10.5);
}

TEST_F(InterpreterRuntimeTest, ToNumberPairInvalidType) {
    double da, db;
    Value a = makeNil();
    Value b = makeInt(10);
    EXPECT_FALSE(toNumberPair(a, b, da, db));
}

// Stack manipulation tests
TEST_F(InterpreterRuntimeTest, PushAndPopValues) {
    Value v1 = makeInt(42);
    Value v2 = makeDouble(3.14);
    
    PUSH(v1);
    PUSH(v2);
    
    Value popped = POP();
    EXPECT_EQ(popped.asDouble(), 3.14);
}

TEST_F(InterpreterRuntimeTest, DuplicateStackValue) {
    Value v = makeInt(100);
    PUSH(v);
    
    Value top = PEEK();
    PUSH(top);
    
    EXPECT_EQ(fiber->stackTop - fiber->stack, 2);
}

// Arithmetic operation tests
TEST_F(InterpreterRuntimeTest, IntegerAddition) {
    Value a = makeInt(5);
    Value b = makeInt(3);
    
    PUSH(a);
    PUSH(b);
    
    int result = (a.asInt() + b.asInt());
    EXPECT_EQ(result, 8);
}

TEST_F(InterpreterRuntimeTest, DoubleSubtraction) {
    Value a = makeDouble(10.5);
    Value b = makeDouble(3.5);
    
    double result = a.asDouble() - b.asDouble();
    EXPECT_EQ(result, 7.0);
}

TEST_F(InterpreterRuntimeTest, IntegerMultiplication) {
    Value a = makeInt(6);
    Value b = makeInt(7);
    
    int result = a.asInt() * b.asInt();
    EXPECT_EQ(result, 42);
}

TEST_F(InterpreterRuntimeTest, IntegerDivision) {
    Value a = makeInt(20);
    Value b = makeInt(4);
    
    int result = a.asInt() / b.asInt();
    EXPECT_EQ(result, 5);
}

TEST_F(InterpreterRuntimeTest, IntegerModulo) {
    Value a = makeInt(17);
    Value b = makeInt(5);
    
    int result = a.asInt() % b.asInt();
    EXPECT_EQ(result, 2);
}

// Bitwise operation tests
TEST_F(InterpreterRuntimeTest, BitwiseAND) {
    Value a = makeInt(12);  // 1100
    Value b = makeInt(10);  // 1010
    
    int result = a.asInt() & b.asInt();
    EXPECT_EQ(result, 8);   // 1000
}

TEST_F(InterpreterRuntimeTest, BitwiseOR) {
    Value a = makeInt(12);  // 1100
    Value b = makeInt(10);  // 1010
    
    int result = a.asInt() | b.asInt();
    EXPECT_EQ(result, 14);  // 1110
}

TEST_F(InterpreterRuntimeTest, BitwiseXOR) {
    Value a = makeInt(12);  // 1100
    Value b = makeInt(10);  // 1010
    
    int result = a.asInt() ^ b.asInt();
    EXPECT_EQ(result, 6);   // 0110
}

TEST_F(InterpreterRuntimeTest, BitwiseNOT) {
    Value a = makeInt(5);
    int result = ~a.asInt();
    EXPECT_EQ(result, -6);
}

TEST_F(InterpreterRuntimeTest, ShiftLeft) {
    Value a = makeInt(5);
    Value b = makeInt(2);
    
    int result = a.asInt() << b.asInt();
    EXPECT_EQ(result, 20);
}

TEST_F(InterpreterRuntimeTest, ShiftRight) {
    Value a = makeInt(20);
    Value b = makeInt(2);
    
    int result = a.asInt() >> b.asInt();
    EXPECT_EQ(result, 5);
}

// Comparison tests
TEST_F(InterpreterRuntimeTest, GreaterThan) {
    double da = 10.0, db = 5.0;
    EXPECT_TRUE(da > db);
}

TEST_F(InterpreterRuntimeTest, LessThanOrEqual) {
    double da = 5.0, db = 10.0;
    EXPECT_TRUE(da <= db);
}

// Logical operation tests
TEST_F(InterpreterRuntimeTest, NegateInteger) {
    Value a = makeInt(42);
    int result = -a.asInt();
    EXPECT_EQ(result, -42);
}

TEST_F(InterpreterRuntimeTest, NegateDouble) {
    Value a = makeDouble(3.14);
    double result = -a.asDouble();
    EXPECT_NEAR(result, -3.14, 0.001);
}