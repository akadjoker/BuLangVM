#pragma once
#include <cassert>
#include "value.hpp"

class TestValue
{
public:
    static void runAllTests()
    {
        testTypeChecks();
        testNumericConversions();
        testBoolConversions();
        testValuesEqual();
        testIsTruthy();
    }

private:
    static void testTypeChecks()
    {
        Value v;
        v.type = ValueType::INT;
        v.as.integer = 42;
        assert(v.isInt());
        assert(v.isNumber());
        assert(!v.isString());
        assert(!v.isNil());

        v.type = ValueType::NIL;
        assert(v.isNil());
        assert(!v.isNumber());

        v.type = ValueType::BOOL;
        v.as.boolean = true;
        assert(v.isBool());
    }

    static void testNumericConversions()
    {
        Value v;
        
        // Test asInt
        v.type = ValueType::DOUBLE;
        v.as.number = 42.7;
        assert(v.asInt() == 42);

        // Test asFloat
        v.type = ValueType::INT;
        v.as.integer = 10;
        assert(v.asFloat() == 10.0f);

        // Test asDouble
        v.type = ValueType::INT;
        v.as.integer = 5;
        assert(v.asDouble() == 5.0);

        // Test asUInt
        v.type = ValueType::INT;
        v.as.integer = 100;
        assert(v.asUInt() == 100u);

        // Test asByte
        v.type = ValueType::INT;
        v.as.integer = 255;
        assert(v.asByte() == 255);
    }

    static void testBoolConversions()
    {
        Value v;

        // Zero is false
        v.type = ValueType::INT;
        v.as.integer = 0;
        assert(!v.asBool());

        // Non-zero is true
        v.as.integer = 1;
        assert(v.asBool());

        // NIL is false
        v.type = ValueType::NIL;
        assert(!v.asBool());

        // Objects are truthy
        v.type = ValueType::ARRAY;
        v.as.array = (ArrayInstance *)0x1;
        assert(v.asBool());
    }

    static void testValuesEqual()
    {
        Value a, b;

        // Numbers with different types but same value
        a.type = ValueType::INT;
        a.as.integer = 42;
        b.type = ValueType::DOUBLE;
        b.as.number = 42.0;
        assert(valuesEqual(a, b));

        // Different number values
        a.type = ValueType::INT;
        a.as.integer = 42;
        b.type = ValueType::INT;
        b.as.integer = 43;
        assert(!valuesEqual(a, b));

        // NIL equals NIL
        a.type = ValueType::NIL;
        b.type = ValueType::NIL;
        assert(valuesEqual(a, b));

        // Different types don't equal
        a.type = ValueType::INT;
        a.as.integer = 0;
        b.type = ValueType::NIL;
        assert(!valuesEqual(a, b));
    }

    static void testIsTruthy()
    {
        Value v;

        // NIL is falsey
        v.type = ValueType::NIL;
        assert(!isTruthy(v));

        // False bool is falsey
        v.type = ValueType::BOOL;
        v.as.boolean = false;
        assert(!isTruthy(v));

        // True bool is truthy
        v.as.boolean = true;
        assert(isTruthy(v));

        // Zero int is falsey
        v.type = ValueType::INT;
        v.as.integer = 0;
        assert(!isTruthy(v));

        // Non-zero int is truthy
        v.as.integer = 42;
        assert(isTruthy(v));

        // Zero double is falsey
        v.type = ValueType::DOUBLE;
        v.as.number = 0.0;
        assert(!isTruthy(v));

        // Non-zero double is truthy
        v.as.number = 3.14;
        assert(isTruthy(v));
    }
};