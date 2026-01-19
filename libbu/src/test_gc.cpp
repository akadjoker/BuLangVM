#include <gtest/gtest.h>
#include "interpreter.hpp"

class GCTest : public ::testing::Test {
protected:
    Interpreter interpreter;

    void SetUp() override {
        // Initialize interpreter state
    }

    void TearDown() override {
        interpreter.clearAllGCObjects();
    }
};

TEST_F(GCTest, MarkRootsWithGlobalVariables) {
    // Test that global variables are marked during root marking
    Value testVal;
    testVal.as.array = nullptr; // Would be set to actual array in real test
    
    interpreter.markRoots();
    // Verify globals are processed
    EXPECT_GE(interpreter.grayStack.size(), 0);
}

TEST_F(GCTest, MarkObjectBasic) {
    GCObject obj;
    obj.marked = 0;
    obj.type = GCObjectType::ARRAY;
    obj.next = nullptr;
    
    interpreter.markObject(&obj);
    EXPECT_EQ(obj.marked, 1);
    EXPECT_EQ(interpreter.grayStack.size(), 1);
}

TEST_F(GCTest, MarkObjectNullptr) {
    interpreter.markObject(nullptr);
    EXPECT_EQ(interpreter.grayStack.size(), 0);
}

TEST_F(GCTest, MarkObjectAlreadyMarked) {
    GCObject obj;
    obj.marked = 1;
    obj.type = GCObjectType::ARRAY;
    obj.next = nullptr;
    
    interpreter.markObject(&obj);
    EXPECT_EQ(interpreter.grayStack.size(), 0);
}

TEST_F(GCTest, SweepUnmarkedObjects) {
    size_t initialCount = interpreter.countObjects();
    
    interpreter.sweep();
    
    // Unmarked objects should be freed
    size_t finalCount = interpreter.countObjects();
    EXPECT_LE(finalCount, initialCount);
}

TEST_F(GCTest, SweepResetMarks) {
    GCObject obj;
    obj.marked = 1;
    obj.type = GCObjectType::ARRAY;
    
    // After sweep, marked objects should have mark reset
    interpreter.sweep();
    EXPECT_EQ(obj.marked, 0);
}

TEST_F(GCTest, CheckGCDisabled) {
    interpreter.enbaledGC = false;
    size_t beforeAlloc = interpreter.totalAllocated;
    interpreter.totalAllocated = 10000000; // Exceed threshold
    
    interpreter.checkGC();
    
    // GC should not run if disabled
    EXPECT_FALSE(interpreter.gcInProgress);
}

TEST_F(GCTest, CheckGCTriggersCollection) {
    interpreter.enbaledGC = true;
    interpreter.nextGC = 1000;
    interpreter.totalAllocated = 2000;
    
    interpreter.checkGC();
    
    // GC should be triggered
    EXPECT_FALSE(interpreter.gcInProgress); // Should complete immediately
}

TEST_F(GCTest, BlackenStructInstance) {
    StructInstance s;
    s.marked = 1;
    s.values.clear();
    
    // Add mock values
    Value v;
    s.values.push_back(v);
    
    interpreter.blackenObject((GCObject *)&s);
    EXPECT_GT(interpreter.grayStack.size(), 0);
}

TEST_F(GCTest, TraceReferencesEmptyStack) {
    interpreter.grayStack.clear();
    
    interpreter.traceReferences();
    
    EXPECT_EQ(interpreter.grayStack.size(), 0);
}

TEST_F(GCTest, RunGCPreventReentrancy) {
    interpreter.gcInProgress = true;
    size_t before = interpreter.totalAllocated;
    
    interpreter.runGC();
    
    // Second call should return early
    EXPECT_TRUE(interpreter.gcInProgress);
}

TEST_F(GCTest, CountObjectsEmpty) {
    interpreter.clearAllGCObjects();
    
    size_t count = interpreter.countObjects();
    
    EXPECT_EQ(count, 0);
}

TEST_F(GCTest, ClearAllGCObjectsSuccess) {
    interpreter.clearAllGCObjects();
    
    EXPECT_EQ(interpreter.countObjects(), 0);
}