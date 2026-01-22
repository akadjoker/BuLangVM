
#include "interpreter.hpp"
#include <cmath>
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

// ========================================
// NATIVE CLASS: Vector2D para testes de herança
// ========================================

struct Vector2DData {
    float x;
    float y;
};

void* Vector2D_constructor(Interpreter* vm, int argc, Value* args) {
    Vector2DData* data = new Vector2DData();
    if (argc >= 2) {
        data->x = (float)args[0].asNumber();
        data->y = (float)args[1].asNumber();
    } else {
        data->x = 0.0f;
        data->y = 0.0f;
    }
    return data;
}

void Vector2D_destructor(Interpreter* vm, void* userData) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    delete data;
}

Value Vector2D_getX(Interpreter* vm, void* userData) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    return vm->makeFloat(data->x);
}

void Vector2D_setX(Interpreter* vm, void* userData, Value value) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    data->x = (float)value.asNumber();
}

Value Vector2D_getY(Interpreter* vm, void* userData) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    return vm->makeFloat(data->y);
}

void Vector2D_setY(Interpreter* vm, void* userData, Value value) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    data->y = (float)value.asNumber();
}

Value Vector2D_length(Interpreter* vm, void* userData, int argc, Value* args) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    float len = std::sqrt(data->x * data->x + data->y * data->y);
    return vm->makeFloat(len);
}

Value Vector2D_add(Interpreter* vm, void* userData, int argc, Value* args) {
    Vector2DData* data = static_cast<Vector2DData*>(userData);
    if (argc >= 2) {
        data->x += (float)args[0].asNumber();
        data->y += (float)args[1].asNumber();
    }
    return vm->makeNil();
}

void registerTestNativeClasses(Interpreter* vm) {
    NativeClassDef* vec2 = vm->registerNativeClass("Vector2D", Vector2D_constructor, Vector2D_destructor, 2);
    vm->addNativeProperty(vec2, "x", Vector2D_getX, Vector2D_setX);
    vm->addNativeProperty(vec2, "y", Vector2D_getY, Vector2D_setY);
    vm->addNativeMethod(vec2, "length", Vector2D_length);
    vm->addNativeMethod(vec2, "add", Vector2D_add);
}

// ========================================
// TESTE: Criar instância de classe script a partir do C++
// ========================================

void testCreateScriptClassFromCpp(Interpreter* vm) {
    printf("\n=== TESTE C++: Criar instância de classe script ===\n");
    
    // Primeiro compila o código que define a classe
    const char* code = R"(
        class Enemy : Vector2D {
            var id;
            var damage;
            def init(eid, dmg) {
                self.id = eid;
                self.damage = dmg;
                self.x = 0.0;
                self.y = 0.0;
            }
            def attack() {
                return self.damage * 2;
            }
        }
    )";
    
    if (!vm->run(code, false)) {
        printf("  ✗ Falhou a compilar a classe\n");
        return;
    }
    
    // Agora cria uma instância a partir do C++
    Value args[2];
    args[0] = vm->makeInt(42);      // id
    args[1] = vm->makeInt(100);     // damage
    
    Value enemyValue = vm->createClassInstance("Enemy", 2, args);
    
    if (enemyValue.isNil()) {
        printf("  ✗ Falhou a criar instância\n");
        return;
    }
    
    printf("  ✓ Instância criada com sucesso!\n");
    
    // Verifica que é uma ClassInstance
    if (!enemyValue.isClassInstance()) {
        printf("  ✗ Não é uma ClassInstance\n");
        return;
    }
    
    ClassInstance* enemy = enemyValue.asClassInstance();
    printf("  ✓ É uma ClassInstance do tipo '%s'\n", enemy->klass->name->chars());
    
    // Acede aos campos
    // Os campos são: id (index 0), damage (index 1)
    Value idValue = enemy->fields[0];
    Value damageValue = enemy->fields[1];
    
    printf("  → enemy.id = %d\n", (int)idValue.asNumber());
    printf("  → enemy.damage = %d\n", (int)damageValue.asNumber());
    
    if ((int)idValue.asNumber() == 42) {
        printf("  ✓ enemy.id == 42\n");
    } else {
        printf("  ✗ enemy.id != 42\n");
    }
    
    if ((int)damageValue.asNumber() == 100) {
        printf("  ✓ enemy.damage == 100\n");
    } else {
        printf("  ✗ enemy.damage != 100\n");
    }
    
    // Verifica que tem nativeUserData (herda de Vector2D)
    if (enemy->nativeUserData != nullptr) {
        printf("  ✓ Tem nativeUserData (herda de Vector2D)\n");
        
        // Acede às propriedades nativas via o userData
        Vector2DData* vec = static_cast<Vector2DData*>(enemy->nativeUserData);
        printf("  → enemy.x = %.1f, enemy.y = %.1f\n", vec->x, vec->y);
    } else {
        printf("  ✗ Não tem nativeUserData\n");
    }
    
    printf("=== TESTE C++ OK ===\n\n");
}

int main(int argc, char **argv)
{
  Interpreter vm;

  // Regista natives de teste
  vm.registerNative("pass", native_pass, 1);
  vm.registerNative("fail", native_fail, 1);
  vm.registerNative("assert", native_assert, 2);
  vm.registerNative("assert_eq", native_assert_eq, 3);

  // Regista NativeClasses de teste
  registerTestNativeClasses(&vm);

  // Teste: criar instância de classe script a partir do C++
  testCreateScriptClassFromCpp(&vm);

  vm.registerAll();

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