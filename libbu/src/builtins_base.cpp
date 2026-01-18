
#include "interpreter.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include <string>

Value native_print_stack(Interpreter *vm, int argCount, Value *args)
{

  if (argCount == 1)
  {
    Info("%s", args[0].asString()->chars());
  }
  vm->printStack();
  return vm->makeNil();
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

Value native_format(Interpreter *vm, int argCount, Value *args)
{
  if (argCount < 1 || args[0].type != ValueType::STRING)
  {
    vm->runtimeError("format expects string as first argument");
    return vm->makeNil();
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

  return vm->makeString(result.c_str());
}

Value native_write(Interpreter *vm, int argCount, Value *args)
{
  if (argCount < 1 || args[0].type != ValueType::STRING)
  {
    vm->runtimeError("write expects string as first argument");
    return vm->makeNil();
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
  return vm->makeNil();
}

Value native_input(Interpreter *vm, int argCount, Value *args)
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
    return vm->makeString(buffer);
  }

  return vm->makeNil();
}

Value native_gc(Interpreter *vm, int argCount, Value *args)
{
  vm->runGC();
  return vm->makeNil();
}


Value native_ticks(Interpreter *vm, int argCount, Value *args)
{
  if (argCount != 1 || !args[0].isNumber())
  {
    vm->runtimeError("ticks expects double as argument");
    return vm->makeNil();
  }
  vm->update(args[0].asNumber());
  return vm->makeNil();
}

void Interpreter::registerBase()
{
  registerNative("format", native_format, -1);
  registerNative("write", native_write, -1);
  registerNative("input", native_input, -1);
  registerNative("print_stack", native_print_stack, -1);
  registerNative("ticks", native_ticks, 1);
  registerNative("_gc", native_gc, 0);
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
