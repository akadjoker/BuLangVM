
#include "interpreter.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int testsPassed = 0;
int testsFailed = 0;
std::string currentTestFile;

// interpreter.cpp
void beginTestFile(const char *filename)
{
  currentTestFile = filename;
  testsPassed = 0;
  testsFailed = 0;
}

void endTestFile()
{
  if (testsFailed == 0)
  {
    printf("✅ %s: %d passed\n", currentTestFile.c_str(), testsPassed);
  }
  else
  {
    printf("❌ %s: %d passed, %d failed\n", currentTestFile.c_str(),
           testsPassed, testsFailed);
  }
}

void testPass(const char *name)
{
  testsPassed++;
  printf("  ✓ %s\n", name);
}

void testFail(const char *name, const char *reason)
{
  testsFailed++;
  if (reason)
  {
    printf("  ✗ %s: %s\n", name, reason);
  }
  else
  {
    printf("  ✗ %s\n", name);
  }
}

void getTestStats(int *passed, int *failed)
{
  *passed = testsPassed;
  *failed = testsFailed;
}

void resetTestStats()
{
  testsPassed = 0;
  testsFailed = 0;
}

std::string valueToString(const Value &value)
{

  return valueTypeToString(value.type);
}

static Value native_pass(Interpreter *vm, int argc, Value *args)
{
  String *name = args[0].asString();
  Info("✓ %s", name->chars());
  return vm->makeNil();
}

static Value native_fail(Interpreter *vm, int argc, Value *args)
{
  String *name = args[0].asString();
  Error("✗ %s\n", name->chars());
  return vm->makeNil();
}

static Value native_assert_eq(Interpreter *vm, int argc, Value *args)
{
  if (argc < 3)
  {
    vm->runtimeError("assert_eq() expects 3 arguments");
    return vm->makeNil();
  }

  Value a = args[0];
  Value b = args[1];
  const char *name =
      args[2].isString() ? args[2].asString()->chars() : "equality";

  bool equal = false;

  if (a.type != b.type)
  {
    equal = false;
  }
  else if (a.isInt())
  {
    equal = (a.asInt() == b.asInt());
  }
  else if (a.isDouble())
  {
    equal = (a.asDouble() == b.asDouble());
  }
  else if (a.isBool())
  {
    equal = (a.asBool() == b.asBool());
  }
  else if (a.isString())
  {
    equal = (strcmp(a.asString()->chars(), b.asString()->chars()) == 0);
  }
  else if (a.isNil() && b.isNil())
  {
    equal = true;
  }

  if (equal)
  {
    testPass(name);
  }
  else
  {

    char bufferA[256];
    char bufferB[256];
    valueToBuffer(a, bufferA, sizeof(bufferA));
    valueToBuffer(b, bufferB, sizeof(bufferB));
    char reason[256];
    snprintf(reason, sizeof(reason), "expected '%s', got '%s'",
             bufferA, bufferB);
 
    testFail(name, reason);
  }

  return vm->makeNil();
}

static Value native_assert(Interpreter *vm, int argc, Value *args)
{
  if (argc < 2)
  {
    vm->runtimeError("assert() expects 2 arguments");
    return vm->makeNil();
  }

  bool condition = args[0].isBool() ? args[0].asBool() : !args[0].isNil();
  const char *name =
      args[1].isString() ? args[1].asString()->chars() : "assertion";

  if (condition)
  {
    testPass(name);
  }
  else
  {
    testFail(name, "assertion failed");
  }

  return vm->makeNil();
}

Value native_clock(Interpreter *vm, int argCount, Value *args)
{
  return vm->makeDouble(static_cast<double>(clock()) / CLOCKS_PER_SEC);
}

// Helper: converte Value para string
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

  default:
    out += "<object>";
  }
}

Value native_print_stack(Interpreter *vm, int argCount, Value *args)
{

  if (argCount == 1)
  {
    Info("%s", args[0].asString()->chars());
  }

  vm->printStack();
  return vm->makeNil();
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

  printf("%s", result.c_str());
  return vm->makeNil();
}
int main(int argc, char **argv)
{
  Interpreter vm;

  // Regista natives de teste
  vm.registerNative("pass", native_pass, 1);
  vm.registerNative("fail", native_fail, 1);
  vm.registerNative("assert", native_assert, 2);
  vm.registerNative("assert_eq", native_assert_eq, 3);

  // Regista natives de teste
  vm.registerNative("clock", native_clock, 0);
  vm.registerNative("format", native_format, -1);
  vm.registerNative("write", native_write, -1);
  vm.registerNative("print_stack", native_print_stack, -1);

  int totalPassed = 0;
  int totalFailed = 0;
  int filesRun = 0;
  int filesFailed = 0;

  namespace fs = std::filesystem;
  const fs::path testDir = "scripts/tests";

  if (!fs::exists(testDir))
  {
    printf("Error: Test directory '%s' does not exist\n", testDir.c_str());
    return 1;
  }

  printf("🧪 Running BuLang Tests\n");
  printf("======================\n\n");

  for (auto &entry : fs::directory_iterator(testDir))
  {
    if (!entry.is_regular_file())
      continue;
    if (entry.path().extension() != ".bu")
      continue;

    std::string path = entry.path().string();
    std::string filename = entry.path().filename().string();

    // Lê código
    std::ifstream file(path);
    if (!file)
    {
      printf("❌ Failed to open %s\n", filename.c_str());
      filesFailed++;
      continue;
    }

    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    // Inicia teste
    beginTestFile(filename.c_str());
    filesRun++;

    // Compila
    if (!vm.run(code.c_str(), false))
    {
      printf("❌ %s: Compilation failed\n\n", filename.c_str());
      filesFailed++;
      continue;
    }

    // Executa (até todos os processos morrerem)
    int maxFrames = 50000; // Safety limit
    int frame = 0;

    while (vm.getTotalAliveProcesses() && frame < maxFrames)
    {
      vm.update(0.016f);
      frame++;
    }

    if (frame >= maxFrames)
    {
      printf("⚠️  Warning: Test hit frame limit (%d frames)\n", maxFrames);
    }

    // Finaliza teste
    endTestFile();

    int passed, failed;
    getTestStats(&passed, &failed);

    totalPassed += passed;
    totalFailed += failed;

    if (failed > 0)
    {
      filesFailed++;
    }

    printf("\n");
  }

  // Sumário
  printf("======================\n");
  printf("📊 Test Summary\n");
  printf("======================\n");
  printf("Files:  %d run, %d failed\n", filesRun, filesFailed);
  printf("Tests:  %d passed, %d failed\n", totalPassed, totalFailed);
  printf("======================\n");

  if (totalFailed == 0 && filesFailed == 0)
  {
    printf("✅ All tests passed!\n");
    return 0;
  }
  else
  {
    printf("❌ Some tests failed\n");
    return 1;
  }
}