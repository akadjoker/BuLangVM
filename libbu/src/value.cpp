#include "value.hpp"
#include "interpreter.hpp"
#include "pool.hpp"
#include "platform.hpp"
#include <stdarg.h>

#include "value.hpp"
#include "interpreter.hpp"
#include "pool.hpp"
#include "platform.hpp"
#include <stdarg.h>

Value::Value() : type(ValueType::NIL)
{
    as.byte = 0;
}

 

// // Unpack
// uint8 getType(Value v) {
//     return (v.as.integer >> 24) & 0xFF;
// }
// uint16 getModuleId(Value v) {
//     return (v.as.integer >> 12) & 0xFFF;
// }
// uint16 getFuncId(Value v) {
//     return v.as.integer & 0xFFF;
// }



const char *typeToString(ValueType type)
{
    switch (type)
    {
    case ValueType::NIL:
        return "nil";
    case ValueType::BOOL:
        return "bool";
    case ValueType::BYTE:
        return "byte";
    case ValueType::INT:
        return "int";
    case ValueType::UINT:
        return "uint";
    case ValueType::FLOAT:
        return "float";
    case ValueType::DOUBLE:
        return "double";
    case ValueType::STRING:
        return "string";
    case ValueType::FUNCTION:
        return "function";
    case ValueType::NATIVE:
        return "native";
    case ValueType::PROCESS:
        return "process";
    case ValueType::ARRAY:
        return "array";
    case ValueType::MAP:
        return "map";
    case ValueType::STRUCT:
        return "struct";
    case ValueType::STRUCTINSTANCE:
        return "struct_instance";
    case ValueType::CLASSINSTANCE:
        return "class_instance";
    case ValueType::NATIVECLASSINSTANCE:
        return "native_class_instance";
    case ValueType::NATIVESTRUCTINSTANCE:
        return "native_struct_instance";
    default:
        return "unknown";
    }
}

void printValue(const Value &value)
{
    switch (value.type)
    {
    case ValueType::NIL:
        OsPrintf("nil");
        break;
    case ValueType::BOOL:
        OsPrintf("%s", value.as.boolean ? "true" : "false");
        break;
    case ValueType::BYTE:
        OsPrintf("%d", value.as.byte);
        break;
    case ValueType::INT:
        OsPrintf("%d", value.as.integer);
        break;
    case ValueType::UINT:
        OsPrintf("%u", value.as.unsignedInteger);
        break;
    case ValueType::FLOAT:
        OsPrintf("%f.4f", value.as.real);
        break;
    case ValueType::DOUBLE:
        OsPrintf("%f.4f", value.as.number);
        break;
    case ValueType::STRING:
    {
        String *str = value.as.string;
        const char *chars = str->chars();
        size_t len = str->length();

        // Se termina com '\n', imprime sem ele
        if (len > 0 && chars[len - 1] == '\n')
        {
            OsPrintf("%.*s", (int)(len - 1), chars); // Imprime len-1 chars
        }
        else
        {
            OsPrintf("%s", chars); // Imprime normal
        }
        break;
    }
    case ValueType::FUNCTION:
    {
        int Id = value.asFunctionId();
        OsPrintf("<function %d>", Id);
        break;
    }
    case ValueType::NATIVE:
        OsPrintf("<native>");
        break;
    case ValueType::PROCESS:
        OsPrintf("<process>");
        break;

    case ValueType::ARRAY:
    {
        ArrayInstance *arr = value.asArray();
        OsPrintf("[");
        for (int i = 0; i < (int)arr->values.size(); i++)
        {
            printValue(arr->values[i]);
            if (i < (int)arr->values.size() - 1)
                OsPrintf(", ");
        }
        OsPrintf("]");
        break;
    }
    case ValueType::MAP:
    {

        MapInstance *map = value.asMap();
        OsPrintf("{");

        int i = 0;
        map->table.forEach([&](String *key, Value val)
                           {
        if (i > 0) OsPrintf(", ");
        OsPrintf("%s: ", key->chars());
        printValue(val);
        i++; });

        OsPrintf("}");
        break;
    }

    case ValueType::STRUCT:
    {
        int Id = value.asStructId();
        OsPrintf("<struct %d>", Id);
        break;
    }
    case ValueType::STRUCTINSTANCE:
    {
        StructInstance *instance = value.as.sInstance;
        OsPrintf("struct '%s' [", instance->def->name->chars());

        bool first = true;

        instance->def->names.forEach([&](String *key, int fieldIndex)
                                     {
             if (!first) 
            {
                OsPrintf(", ");
            }
            first = false;

            OsPrintf("%s = ", key->chars());
            printValue(instance->values[fieldIndex]); });

        OsPrintf("]\n");
        break;
    }
    case ValueType::CLASS:
    {
        int classId = value.asClassId();
        OsPrintf("<class %d>", classId);
        break;
    }
    case ValueType::CLASSINSTANCE:
    {
        ClassInstance *inst = value.asClassInstance();
        OsPrintf("<instance %s>", inst->klass->name->chars());
        break;
    }
    case ValueType::NATIVECLASSINSTANCE:
    {
        NativeClassInstance *inst = value.as.sClassInstance;
        OsPrintf("<native_instance %s>", inst->klass->name->chars());
        break;
    }
    case ValueType::NATIVESTRUCTINSTANCE:
    {
        NativeStructInstance *inst = value.as.sNativeStruct;
        OsPrintf("<native_struct_instance %s>", inst->def->name->chars());
        break;
    }
    case ValueType::POINTER:
    {

        OsPrintf("<pointer %p>", value.as.pointer);
        break;
    }
    case ValueType::MODULEREFERENCE:
    {
        OsPrintf("<module_reference %d %d %d>", value.as.unsignedInteger >> 24, (value.as.unsignedInteger >> 12) & 0xFFF, value.as.unsignedInteger & 0xFFF);
        break;
    }

    default:
        OsPrintf("<?%ld?>", (int)(value.type));
        break;
    }
}

void printValueNl(const Value &value)
{
    printValue(value);
    OsPrintf("\n");
}