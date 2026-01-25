
#include "interpreter.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include <string>

int native_print_stack(Interpreter *vm, int argCount, Value *args)
{

  if (argCount == 1)
  {
    Info("%s", args[0].asString()->chars());
  }
  vm->printStack();
  return 0;
}

static void valueToString(const Value &v, std::string &out)
{
  char buffer[256];

  switch (v.type)
  {
  case ValueType::NIL:
    out += "nil";
    break;
  case ValueType::BOOL:
    out += v.as.boolean ? "true" : "false";
    break;
  case ValueType::BYTE:
    snprintf(buffer, 256, "%u", v.as.byte);
    out += buffer;
    break;
  case ValueType::INT:
    snprintf(buffer, 256, "%d", v.as.integer);
    out += buffer;
    break;
  case ValueType::UINT:
    snprintf(buffer, 256, "%u", v.as.unsignedInteger);
    out += buffer;
    break;
  case ValueType::FLOAT:
    snprintf(buffer, 256, "%.2f", v.as.real);
    out += buffer;
    break;
  case ValueType::DOUBLE:
    snprintf(buffer, 256, "%.2f", v.as.number);
    out += buffer;
    break;
  case ValueType::STRING:
  {
    out += v.asStringChars();
    break;
  }
  case ValueType::ARRAY:
    out += "[array]";
    break;
  case ValueType::MAP:
    out += "{map}";
    break;
  case ValueType::BUFFER:
    out += "[buffer]";
    break;
  default:
    out += "<unknown>";
  }
}

int native_format(Interpreter *vm, int argCount, Value *args)
{
  if (argCount < 1 || args[0].type != ValueType::STRING)
  {
    vm->runtimeError("format expects string as first argument");
    return 0;
  }

  const char *fmt = args[0].asStringChars();
  std::string result;
  int argIndex = 1;

  for (int i = 0; fmt[i] != '\0'; i++)
  {
    if (fmt[i] == '{' && fmt[i + 1] == '}')
    {
      if (argIndex < argCount)
      {
        valueToString(args[argIndex++], result);
      }
      i++;
    }
    else
    {
      result += fmt[i];
    }
  }

  vm->push(vm->makeString(result.c_str()));
  return 1;
}

int native_write(Interpreter *vm, int argCount, Value *args)
{
  if (argCount < 1 || args[0].type != ValueType::STRING)
  {
    vm->runtimeError("write expects string as first argument");
    return 0;
  }

  const char *fmt = args[0].asStringChars();
  std::string result;
  int argIndex = 1;

  for (int i = 0; fmt[i] != '\0'; i++)
  {
    if (fmt[i] == '{' && fmt[i + 1] == '}')
    {
      if (argIndex < argCount)
      {
        valueToString(args[argIndex++], result);
      }
      i++;
    }
    else
    {
      result += fmt[i];
    }
  }

  OsPrintf("%s", result.c_str());
  return 0;
}

int native_input(Interpreter *vm, int argCount, Value *args)
{
  if (argCount > 0 && args[0].isString())
  {
    printf("%s", args[0].asStringChars());
  }

  char buffer[1024];
  if (fgets(buffer, sizeof(buffer), stdin) != NULL)
  {
    size_t length = strlen(buffer);

    // Remove o \n do final
    if (length > 0 && buffer[length - 1] == '\n')
    {
      buffer[length - 1] = '\0';
    }
    vm->push(vm->makeString(buffer));
    return 1;
  }

  return 0;
}

int native_gc(Interpreter *vm, int argCount, Value *args)
{
  vm->runGC();
  return 0;
}


int native_ticks(Interpreter *vm, int argCount, Value *args)
{
  if (argCount != 1 || !args[0].isNumber())
  {
    vm->runtimeError("ticks expects double as argument");
    return 0;
  }
  vm->update(args[0].asNumber());
  return 0;
}

// // Teste multi-return com native function normal
// int native_multi2(Interpreter *vm, int argCount, Value *args)
// {
//   vm->push(vm->makeInt(100));
//   vm->push(vm->makeInt(200));
//   return 2;
// }

// // ===========================================
// // TEST NATIVE CLASS - Para testar edge cases
// // ===========================================
// struct TestNativeData { int value; };

// void* TestNative_ctor(Interpreter* vm, int argc, Value* args) {
//     TestNativeData* data = new TestNativeData();
//     data->value = argc > 0 ? args[0].asInt() : 0;
//     return data;
// }

// void TestNative_dtor(Interpreter* vm, void* userData) {
//     delete (TestNativeData*)userData;
// }

// // Retorna 0 valores (void)
// int TestNative_void(Interpreter* vm, void* userData, int argc, Value* args) {
//     Info("TestNative.void_method() called");
//     return 0;
// }

// // Retorna 1 valor
// int TestNative_one(Interpreter* vm, void* userData, int argc, Value* args) {
//     vm->push(vm->makeInt(42));
//     return 1;
// }

// // Retorna 2 valores
// int TestNative_two(Interpreter* vm, void* userData, int argc, Value* args) {
 
//     vm->push(vm->makeInt(100));
//     vm->push(vm->makeInt(200));
 
//     return 2;
// }

// // Retorna 3 valores
// int TestNative_three(Interpreter* vm, void* userData, int argc, Value* args) {
//     vm->push(vm->makeString("hello"));
//     vm->push(vm->makeInt(42));
//     vm->push(vm->makeBool(true));
//     return 3;
// }

// // Getter para propriedade
// Value TestNative_getValue(Interpreter* vm, void* userData) {
//     TestNativeData* data = (TestNativeData*)userData;
//     return vm->makeInt(data->value);
// }

// // Setter para propriedade
// void TestNative_setValue(Interpreter* vm, void* userData, Value val) {
//     TestNativeData* data = (TestNativeData*)userData;
//     data->value = val.asInt();
// }


// int native_teste1(Interpreter *vm, int argCount, Value *args)
// {
//     vm->pushInt(99);
//     return 1;
// }
// int native_teste2(Interpreter *vm, int argCount, Value *args)
// {
//     vm->pushInt(99);
//     vm->pushInt(69);
//     return 2;
// }



void Interpreter::registerBase()
{
  registerNative("format", native_format, -1);
  registerNative("write", native_write, -1);
  registerNative("input", native_input, -1);
  registerNative("print_stack", native_print_stack, -1);
  registerNative("ticks", native_ticks, 1);
  registerNative("_gc", native_gc, 0);

  // registerNative("teste1", native_teste1, 0);
  // registerNative("teste2", native_teste2, 0);


  // // Registar classe nativa de teste
  // NativeClassDef* testClass = registerNativeClass("TestNative", TestNative_ctor, TestNative_dtor, 1);
  // addNativeMethod(testClass, "void_method", TestNative_void);
  // addNativeMethod(testClass, "one", TestNative_one);
  // addNativeMethod(testClass, "two", TestNative_two);
  // addNativeMethod(testClass, "three", TestNative_three);
  // addNativeProperty(testClass, "value", TestNative_getValue, TestNative_setValue);
}

void Interpreter::registerAll()
{
  registerBase();

#ifdef BU_ENABLE_MATH
  registerMath();
#endif

#ifdef BU_ENABLE_OS

  registerOS();

#endif

#ifdef BU_ENABLE_PATH

  registerPath();

#endif

#ifdef BU_ENABLE_TIME

  registerTime();

#endif

#ifdef BU_ENABLE_FILE_IO
  registerFS();
  registerFile();
#endif

#ifdef BU_ENABLE_SOCKETS
  registerSocket();
#endif
}
