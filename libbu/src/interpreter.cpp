#include "interpreter.hpp"
#include "compiler.hpp"
#include "debug.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include <stdarg.h>

Interpreter::Interpreter()
{
  compiler = new Compiler(this);
  asEnded = false;
  setPrivateTable();
  staticNames[STATIC_PUSH] = createString("push");
  staticNames[STATIC_POP] = createString("pop");
  staticNames[STATIC_BACK] = createString("back");
  staticNames[STATIC_LENGTH] = createString("length");
  staticNames[STATIC_CLEAR] = createString("clear");

  staticNames[STATIC_HAS] = createString("has");
  staticNames[STATIC_REMOVE] = createString("remove");
  staticNames[STATIC_KEYS] = createString("keys");
  staticNames[STATIC_VALUES] = createString("values");

  staticNames[STATIC_UPPER] = createString("upper");
  staticNames[STATIC_LOWER] = createString("lower");
  staticNames[STATIC_CONCAT] = createString("concat");
  staticNames[STATIC_SUB] = createString("sub");
  staticNames[STATIC_REPLACE] = createString("replace");
  staticNames[STATIC_AT] = createString("at");
  staticNames[STATIC_CONTAINS] = createString("contains");
  staticNames[STATIC_TRIM] = createString("trim");
  staticNames[STATIC_STARTWITH] = createString("startswith");
  staticNames[STATIC_ENDWITH] = createString("endswith");
  staticNames[STATIC_INDEXOF] = createString("indexof");
  staticNames[STATIC_REPEAT] = createString("repeat");
  staticNames[STATIC_INIT] = createString("init");
}

void Interpreter::freeInstances()
{
  for (size_t i = 0; i < nativeInstances.size(); i++)
  {
    NativeClassInstance *a = nativeInstances[i];
    a->klass->destructor(this, a->userData); // so depois apagamos as def
    freeNativeClass(a);
  }
  nativeInstances.clear();

  for (size_t i = 0; i < structInstances.size(); i++)
  {
    StructInstance *a = structInstances[i];
    freeStruct(a);
  }
  structInstances.clear();

  for (size_t i = 0; i < classInstances.size(); i++)
  {
    ClassInstance *a = classInstances[i];
    freeClass(a);
  }
  classInstances.clear();

  for (size_t i = 0; i < mapInstances.size(); i++)
  {
    MapInstance *a = mapInstances[i];
    freeMap(a);
  }
  mapInstances.clear();

  for (size_t i = 0; i < arrayInstances.size(); i++)
  {
    ArrayInstance *a = arrayInstances[i];
    freeArray(a);
  }
  arrayInstances.clear();

  for (size_t i = 0; i < nativeStructInstances.size(); i++)
  {
    NativeStructInstance *a = nativeStructInstances[i];
    if (a->def->destructor)
    {
      a->def->destructor(this, a->data);
      arena.Free(a->data, a->def->structSize);
    }
    freeNativeStruct(a);
  }
  nativeStructInstances.clear();
}

void Interpreter::freeBlueprints()
{

  for (size_t j = 0; j < classes.size(); j++)
  {
    ClassDef *proc = classes[j];
    delete proc;
  }
  classes.clear();
  for (size_t i = 0; i < nativeStructs.size(); i++)
  {
    NativeStructDef *a = nativeStructs[i];
    delete a;
  }
  nativeStructs.clear();

  for (size_t i = 0; i < natives.size(); i++)
  {
    NativeDef &native = natives[i];
  }
  natives.clear();

  for (size_t i = 0; i < structs.size(); i++)
  {
    StructDef *a = structs[i];

    delete a;
  }
  structs.clear();
  for (size_t j = 0; j < nativeClasses.size(); j++)
  {
    NativeClassDef *proc = nativeClasses[j];
    delete proc;
  }
  nativeClasses.clear();

  for (size_t j = 0; j < processes.size(); j++)
  {
    ProcessDef *proc = processes[j];
    proc->release();
    delete proc;
  }
  processes.clear();
}

Interpreter::~Interpreter()
{

  Info("VM shutdown");
  Info("Memory allocated : %s", formatBytes(totalAllocated));
  Info("Classes          : %zu", getTotalClasses());
  Info("Structs          : %zu", getTotalStructs());
  Info("Arrays           : %zu", getTotalArrays());
  Info("Maps             : %zu", getTotalMaps());
  Info("Native classes   : %zu", getTotalNativeClasses());
  Info("Native structs   : %zu", getTotalNativeStructs());

  for (size_t i = 0; i < modules.size(); i++)
  {
    ModuleDef *mod = modules[i];
    totalAllocated -= sizeof(ModuleDef);
    delete mod;
  }

  modules.clear();

  delete compiler;

  // instances
  freeInstances();
  // BLUEPRINTS
  freeBlueprints();

  runGC();
  functionsMap.destroy();
  processesMap.destroy();

  for (size_t i = 0; i < functions.size(); i++)
  {
    Function *func = functions[i];
    delete func;
  }
  functions.clear();

  for (size_t i = 0; i < functionsClass.size(); i++)
  {
    Function *func = functionsClass[i];
    delete func;
  }
  functionsClass.clear();

  ProcessPool::instance().clear();

  for (size_t j = 0; j < cleanProcesses.size(); j++)
  {
    Process *proc = cleanProcesses[j];
    proc->release();
    ProcessPool::instance().free(proc);
  }
  cleanProcesses.clear();

  for (size_t i = 0; i < aliveProcesses.size(); i++)
  {
    Process *process = aliveProcesses[i];
    process->release();
    ProcessPool::instance().free(process);
  }

  aliveProcesses.clear();

  Info("Heap stats:");
  arena.Stats();
  arena.Clear();
  Info("String Heap stats:");
  stringPool.clear();
}

void Interpreter::setFileLoader(FileLoaderCallback loader, void *userdata)
{
  compiler->setFileLoader(loader, userdata);
}

NativeClassDef *Interpreter::registerNativeClass(const char *name,
                                                 NativeConstructor ctor,
                                                 NativeDestructor dtor,
                                                 int argCount)
{
  NativeClassDef *klass = new NativeClassDef();
  klass->name = createString(name);
  int id = nativeClasses.size();
  klass->constructor = ctor;
  klass->destructor = dtor;
  klass->argCount = argCount;

  nativeClasses.push(klass);

  // Define global
  globals.set(klass->name, makeNativeClass(id));

  return klass;
}

void Interpreter::addNativeMethod(NativeClassDef *klass, const char *methodName,
                                  NativeMethod method)
{
  String *name = createString(methodName);
  klass->methods.set(name, method);
}

void Interpreter::addNativeProperty(NativeClassDef *klass, const char *propName,
                                    NativeGetter getter, NativeSetter setter)
{
  String *name = createString(propName);

  NativeProperty prop;
  prop.getter = getter;
  prop.setter = setter;

  klass->properties.set(name, prop);
}

Value Interpreter::createNativeStruct(int structId, int argc, Value *args)
{
  NativeStructDef *def = nativeStructs[structId];
  void *data = arena.Allocate(def->structSize);
  std::memset(data, 0, def->structSize);
  if (def->constructor)
  {
    def->constructor(this, data, argc, args);
  }

  Value literal = makeNativeStructInstance();
  NativeStructInstance *instance = literal.asNativeStructInstance();
  instance->def = def;
  instance->data = data;
  return literal;
}

NativeStructDef *Interpreter::registerNativeStruct(const char *name,
                                                   size_t structSize,
                                                   NativeStructCtor ctor,
                                                   NativeStructDtor dtor)
{
  NativeStructDef *klass = new NativeStructDef();
  klass->name = createString(name);
  klass->constructor = ctor;
  klass->destructor = dtor;
  klass->structSize = structSize;
  klass->id = nativeStructs.size();
  nativeStructs.push(klass);
  globals.set(klass->name, makeNativeStruct(klass->id));
  return klass;
}

void Interpreter::addStructField(NativeStructDef *def, const char *fieldName,
                                 size_t offset, FieldType type, bool readOnly)
{
  String *name = createString(fieldName);
  NativeFieldDef field;
  field.offset = offset;
  field.type = type;
  field.readOnly = readOnly;
  if (def->fields.exist(name))
  {
    Warning("Field %s already exists in struct %s", fieldName,
            def->name->chars());
  }
  def->fields.set(name, field);
}

void Interpreter::printStack()
{

   if (currentFiber)
  {
 
    Fiber *fiber = currentFiber;
    if (fiber->stackTop == fiber->stack)
        printf("  (empty)\n");
    else
        printf("          ");
        for (Value *slot = fiber->stack; slot < fiber->stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
  }
}

void Interpreter::disassemble()
{

  printf("\n");
  printf("========================================\n");
  printf("         BYTECODE DUMP\n");
  printf("========================================\n\n");

  // ========== FUNCTIONS ==========
  printf(">>> FUNCTIONS: %zu\n\n", functions.size());

  for (size_t i = 0; i < functions.size(); i++)
  {
    Function *func = functions[i];
    if (!func)
      continue;

    printf("----------------------------------------\n");
    printf("Function #%zu: %s\n", i, func->name->chars());
    printf("  Arity: %d\n", func->arity);
    printf("  Has return: %s\n", func->hasReturn ? "yes" : "no");
    printf("----------------------------------------\n");

    // Constants
    printf("\nConstants (%zu):\n", func->chunk->constants.size());
    for (size_t j = 0; j < func->chunk->constants.size(); j++)
    {
      printf("  [%zu] = ", j);
      printValue(func->chunk->constants[j]);
      printf("\n");
    }

    // Bytecode usando Debug::disassembleInstruction
    printf("\nBytecode:\n");
    for (size_t offset = 0; offset < func->chunk->count;)
    {
      printf("  ");
      offset = Debug::disassembleInstruction(*func->chunk, offset);
    }

    printf("\n");
  }

  // ========== PROCESSES ==========
  printf("\n>>> PROCESSES: %zu\n\n", processes.size());

  for (size_t i = 1; i < processes.size(); i++)
  {
    ProcessDef *proc = processes[i];
    if (!proc)
      continue;

    printf("----------------------------------------\n");
    printf("Process #%zu: %s \n", i, proc->name->chars());
    printf("----------------------------------------\n");

    if (proc->fibers[0].frameCount > 0)
    {
      Function *func = proc->fibers[0].frames[0].func;

      printf("  Function: %s\n", func->name->chars());
      printf("  Arity: %d\n", func->arity);

      // Constants
      printf("\nConstants (%zu):\n", func->chunk->constants.size());
      for (size_t j = 0; j < func->chunk->constants.size(); j++)
      {
        printf("  [%zu] = ", j);
        printValue(func->chunk->constants[j]);
        printf("\n");
      }

      // Bytecode usando Debug::disassembleInstruction
      printf("\nBytecode:\n");
      for (size_t offset = 0; offset < func->chunk->count;)
      {
        printf("  ");
        offset = Debug::disassembleInstruction(*func->chunk, offset);
      }
    }

    printf("\n");
  }

  // ========== NATIVES ==========
  printf("\n>>> NATIVE FUNCTIONS: %zu\n\n", natives.size());

  for (size_t i = 0; i < natives.size(); i++)
  {
    printf("  [%2zu] %-20s (arity: %2d)\n", i, natives[i].name->chars(),
           natives[i].arity);
  }

  printf("\n========================================\n");
  printf("              END OF DUMP\n");
  printf("========================================\n\n");
}

int Interpreter::addGlobal(const char *name, Value value)
{
  String *str = createString(name);
  globals.set(str, value);
  return 0;
}

void Interpreter::print(Value value) { printValue(value); }

Fiber *Interpreter::get_ready_fiber(Process *proc)
{
  int checked = 0;
  int totalFibers = proc->nextFiberIndex;

  if (totalFibers == 0)
    return nullptr;

  // printf("[get_ready_fiber] Checking %d fibers, time=%.3f\n", totalFibers,
  // currentTime);

  while (checked < totalFibers)
  {
    int idx = proc->currentFiberIndex;
    proc->currentFiberIndex = (proc->currentFiberIndex + 1) % totalFibers;

    Fiber *f = &proc->fibers[idx];

    // printf("  Fiber %d: state=%d, resumeTime=%.3f\n", idx, (int)f->state,
    // f->resumeTime);

    checked++;

    if (f->state == FiberState::DEAD)
    {
      //  printf("  -> DEAD, skip\n");
      continue;
    }

    if (f->state == FiberState::SUSPENDED)
    {
      if (currentTime >= f->resumeTime)
      {
        // printf("  -> RESUMING (%.3f >= %.3f)\n", currentTime, f->resumeTime);
        f->state = FiberState::RUNNING;
        return f;
      }
      // printf("  -> Still suspended (wait %.3fms)\n", (f->resumeTime -
      // currentTime) * 1000);
      continue;
    }

    if (f->state == FiberState::RUNNING)
    {
      //  printf("  -> RUNNING, execute\n");
      return f;
    }
  }

  //  printf("  -> No ready fiber\n");
  return nullptr;
}

float Interpreter::getCurrentTime() const { return currentTime; }

static void OsVPrintf(const char *fmt, va_list args)
{
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  OsPrintf("%s", buffer);
}

void Interpreter::runtimeError(const char *format, ...)
{
  hasFatalError_ = true;

  OsPrintf("Runtime Error: ");
  va_list args;
  va_start(args, format);
  OsVPrintf(format, args);
  va_end(args);
  OsPrintf("\n");

#ifdef WDIV_DEBUG
  if (currentFiber->frameCount > 0)
  {
    // CallFrame *frame = &currentFiber->frames[currentFiber->frameCount - 1];
    // Debug::dumpFunction(frame->func);
  }
#endif

  resetFiber();
}

void Interpreter::resetFiber()
{

  if (currentFiber)
  {
    currentFiber->stackTop = currentFiber->stack;
    currentFiber->frameCount = 0;
    currentFiber->state = FiberState::DEAD;
  }
  hasFatalError_ = false;
}

Function *Interpreter::compile(const char *source)
{
  ProcessDef *proc = compiler->compile(source);
  Function *mainFunc = proc->fibers[0].frames[0].func;
  return mainFunc;
}

Function *Interpreter::compileExpression(const char *source)
{
  ProcessDef *proc = compiler->compileExpression(source);
  Function *mainFunc = proc->fibers[0].frames[0].func;
  return mainFunc;
}

bool Interpreter::run(const char *source, bool _dump)
{
  reset();

  ProcessDef *proc = compiler->compile(source);
  if (!proc)
  {
    return false;
  }

  // functionsMap.destroy();
  // processesMap.destroy();
  // nativesMap.destroy();

  if (_dump)
  {
    disassemble();
    // Function *mainFunc = proc->fibers[0].frames[0].func;
    //   Debug::dumpFunction(mainFunc);
  }

  mainProcess = spawnProcess(proc);
  currentProcess = mainProcess;

  Fiber *fiber = &mainProcess->fibers[0];

  //  Debug::disassembleChunk(*fiber->frames[0].func->chunk,"#main");

  run_fiber(fiber);

  return !hasFatalError_;
}
bool Interpreter::compile(const char *source, bool dump)
{
  reset();

  ProcessDef *proc = compiler->compile(source);
  if (!proc)
  {
    return false;
  }

  if (dump)
  {
    disassemble();
    // Function *mainFunc = proc->fibers[0].frames[0].func;
    //   Debug::dumpFunction(mainFunc);
  }

  return !hasFatalError_;
}
void Interpreter::reset()
{

  functionsMap.destroy();
  processesMap.destroy();

  for (size_t i = 0; i < functions.size(); i++)
  {
    Function *func = functions[i];
    delete func;
  }
  functions.clear();

  for (size_t i = 0; i < functionsClass.size(); i++)
  {
    Function *func = functionsClass[i];
    delete func;
  }
  functionsClass.clear();

  for (size_t j = 0; j < processes.size(); j++)
  {
    ProcessDef *proc = processes[j];
    proc->release();
    delete proc;
  }

  processes.clear();
  ProcessPool::instance().clear();

  for (size_t j = 0; j < cleanProcesses.size(); j++)
  {
    Process *proc = cleanProcesses[j];
    proc->release();
    ProcessPool::instance().free(proc);
  }
  cleanProcesses.clear();

  for (size_t i = 0; i < aliveProcesses.size(); i++)
  {
    Process *process = aliveProcesses[i];
    process->release();
    ProcessPool::instance().free(process);
  }

  aliveProcesses.clear();

  structs.clear();
  currentFiber = nullptr;
  currentProcess = nullptr;
  currentTime = 0.0f;
  hasFatalError_ = false;
  asEnded = false;

  compiler->clear();
}
void Interpreter::setHooks(const VMHooks &h) { hooks = h; }

void Interpreter::initFiber(Fiber *fiber, Function *func)
{
  fiber->state = FiberState::RUNNING;
  fiber->resumeTime = 0.0f;

  fiber->stackTop = fiber->stack;

  fiber->ip = func->chunk->code;

  fiber->frameCount = 1;
  fiber->frames[0].func = func;
  fiber->frames[0].ip = nullptr;
  fiber->frames[0].slots = fiber->stack; // Base da stack
}

void Interpreter::setPrivateTable()
{
  privateIndexMap.set("x", 0);
  privateIndexMap.set("y", 1);
  privateIndexMap.set("z", 2);
  privateIndexMap.set("graph", 3);
  privateIndexMap.set("angle", 4);
  privateIndexMap.set("size", 5);
  privateIndexMap.set("flags", 6);
  privateIndexMap.set("id", 7);
  privateIndexMap.set("father", 8);
}

StructDef *Interpreter::addStruct(String *name, int *id)
{

  if (structsMap.exist(name))
  {
    return nullptr;
  }
  StructDef *proc = new StructDef();
  structsMap.set(name, proc);
  proc->index = (int)structs.size();
  *id = (int)structs.size();
  structs.push(proc);
  return proc;
}

StructDef *Interpreter::registerStruct(String *name)
{
  if (structsMap.exist(name))
  {
    return nullptr;
  }

  StructDef *proc = new StructDef();

  proc->name = name;
  proc->argCount = 0;
  structsMap.set(name, proc);
  proc->index = (int)structs.size();

  structs.push(proc);

  return proc;
}

ClassDef *Interpreter::registerClass(String *name)
{
  if (structsMap.exist(name))
  {
    return nullptr;
  }

  ClassDef *proc = new ClassDef();
  proc->name = name;
  classesMap.set(name, proc);

  proc->index = (int)classes.size();

  classes.push(proc);

  return proc;
}

String *Interpreter::createString(const char *str, uint32 len)
{
  return stringPool.create(str, len);
}

String *Interpreter::createString(const char *str)
{
  return stringPool.create(str);
}

bool Interpreter::containsClassDefenition(String *name)
{
  return classesMap.exist(name);
}

bool Interpreter::getClassDefenition(String *name, ClassDef *result)
{
  return classesMap.get(name, &result);
}

bool Interpreter::tryGetClassDefenition(const char *name, ClassDef **out)
{
  String *pName = createString(name);
  bool result = false;
  if (classesMap.get(pName, out))
  {
    result = true;
  }
  return result;
}

void Interpreter::addFiber(Process *proc, Function *func)
{
  if (proc->nextFiberIndex >= MAX_FIBERS)
  {
    runtimeError("Too many fibers in process");
    return;
  }

  int index = proc->nextFiberIndex++;
  initFiber(&proc->fibers[index], func);
}

void Interpreter::addFunctionsClasses(Function *fun)
{
  functionsClass.push(fun);
}

StructInstance::StructInstance() : marked(0), def(nullptr) {}

ArrayInstance::ArrayInstance() : marked(0) {}

MapInstance::MapInstance() : marked(0) {}

ClassInstance::ClassInstance() : marked(0) {}

ClassInstance::~ClassInstance()
{
}

NativeClassInstance::NativeClassInstance() : marked(0), klass(nullptr), userData(nullptr)
{
}

NativeStructInstance::NativeStructInstance() : marked(0), def(nullptr), data(nullptr)
{
}

// bool ClassInstance::getMethod(String *name, Function **out)
// {
//   ClassDef *current = klass;

//   while (current)
//   {
//     if (current->methods.get(name, out))
//     {
//       return true;
//     }
//     current = current->superclass;
//   }

//   return false;
// }

// bool ClassInstance::getMethod(String *name, Function **out)
// {
//     if(klass->methods.get(name, out))
//     {
//         return true;
//     }

//     if (klass->inherited)
//     {
//         if(klass->superclass->methods.get(name,out));
//         {
//             return true;
//         }
//     }
//     return false;
// }

Function *ClassDef::canRegisterFunction(String *pName)
{

  if (methods.exist(pName))
  {
    return nullptr;
  }
  Function *func = new Function();
  func->index = -1;
  func->arity = 0;
  func->hasReturn = false;
  func->name = pName;
  func->chunk = new Code(16);
  methods.set(pName, func);
  return func;
}

StructDef::~StructDef()
{
  names.destroy();
}

ClassDef::~ClassDef()
{
  fieldNames.destroy();
  methods.destroy();
  superclass = nullptr;
}

NativeClassDef::~NativeClassDef()
{
  methods.destroy();
  properties.destroy();
}

void Interpreter::dumpToFile(const char *filename)
{

#ifdef __linux__

  FILE *f = fopen(filename, "w");
  if (!f)
  {
    fprintf(stderr, "Failed to open %s for writing\n", filename);
    return;
  }

  fprintf(f, "========================================\n");
  fprintf(f, "BULANG BYTECODE DUMP\n");
  fprintf(f, "========================================\n\n");

  // Dump global functions
  dumpAllFunctions(f);

  // Dump classes e métodos
  dumpAllClasses(f);

  fprintf(f, "\n========================================\n");
  fprintf(f, "END OF DUMP\n");
  fprintf(f, "========================================\n");

  fclose(f);
  printf("Bytecode dumped to: %s\n", filename);

#endif
}

void Interpreter::dumpAllFunctions(FILE *f)
{
#ifdef __linux__
  fprintf(f, "========================================\n");
  fprintf(f, "GLOBAL FUNCTIONS\n");
  fprintf(f, "========================================\n\n");

  functionsMap.forEach([&](String *name, Function *func)
                       {
        fprintf(f, "\n>>> Function: %s\n", name->chars());
        fprintf(f, "    Arity: %d\n", func->arity);
        fprintf(f, "    Has return: %s\n", func->hasReturn ? "yes" : "no");
        fprintf(f, "    Index: %d\n\n", func->index);
        
        // Constants
        fprintf(f, "  Constants (%zu):\n", func->chunk->constants.size());
        for (size_t i = 0; i < func->chunk->constants.size(); i++) {
            fprintf(f, "    [%zu] = ", i);
            
            Value v = func->chunk->constants[i];
            if (v.isString()) {
                fprintf(f, "\"%s\"", v.asString()->chars());
            } else if (v.isInt()) {
                fprintf(f, "%d", v.asInt());
            } else if (v.isDouble()) {
                fprintf(f, "%.2f", v.asDouble());
            } else if (v.isBool()) {
                fprintf(f, "%s", v.asBool() ? "true" : "false");
            } else if (v.isNil()) {
                fprintf(f, "nil");
            } else {
                fprintf(f, "<value type %d>", (int)v.type);
            }
            fprintf(f, "\n");
        }
        fprintf(f, "\n");
        
        // Bytecode
        fprintf(f, "  Bytecode:\n");
        for (size_t offset = 0; offset < func->chunk->count;) {
            fprintf(f, "    ");
            
            // Salva stdout, redireciona para file temporariamente
            FILE* old_stdout = stdout;
            stdout = f;
            offset = Debug::disassembleInstruction(*func->chunk, offset);
            stdout = old_stdout;
        }
        
        fprintf(f, "\n"); });
#endif
}

void Interpreter::dumpAllClasses(FILE *f)
{
#ifdef __linux__
  fprintf(f, "\n========================================\n");
  fprintf(f, "CLASSES\n");
  fprintf(f, "========================================\n\n");

  classesMap.forEach([&](String *name, ClassDef *klass)
                     {
        fprintf(f, "\n>>> Class: %s\n", name->chars());
        fprintf(f, "    Index: %d\n", klass->index);
        fprintf(f, "    Field count: %d\n", klass->fieldCount);
        fprintf(f, "    Has superclass: %s\n", klass->superclass ? "yes" : "no");
        
        if (klass->superclass) {
            fprintf(f, "    Superclass: %s\n", klass->superclass->name->chars());
        }
        
        // Fields
        fprintf(f, "\n    Fields:\n");
        klass->fieldNames.forEach([&](String* fieldName, uint8_t index) {
            fprintf(f, "      [%u] %s\n", index, fieldName->chars());
        });
        
        // Constructor
        if (klass->constructor) {
            fprintf(f, "\n    >>> Constructor (init)\n");
            fprintf(f, "        Arity: %d\n\n", klass->constructor->arity);
            
            // Constants
            fprintf(f, "      Constants (%zu):\n", 
                   klass->constructor->chunk->constants.size());
            for (size_t i = 0; i < klass->constructor->chunk->constants.size(); i++) {
                fprintf(f, "        [%zu] = ", i);
                
                Value v = klass->constructor->chunk->constants[i];
                if (v.isString()) {
                    fprintf(f, "\"%s\"", v.asString()->chars());
                } else if (v.isInt()) {
                    fprintf(f, "%d", v.asInt());
                } else if (v.isDouble()) {
                    fprintf(f, "%.2f", v.asDouble());
                } else {
                    fprintf(f, "<value>");
                }
                fprintf(f, "\n");
            }
            fprintf(f, "\n");
            
            // Bytecode
            fprintf(f, "      Bytecode:\n");
            for (size_t offset = 0; offset < klass->constructor->chunk->count;) {
                fprintf(f, "        ");
                
                FILE* old_stdout = stdout;
                stdout = f;
                offset = Debug::disassembleInstruction(*klass->constructor->chunk, offset);
                stdout = old_stdout;
            }
        }
        
        // Methods
        fprintf(f, "\n    Methods:\n");
        klass->methods.forEach([&](String* methodName, Function* method) {
            fprintf(f, "\n    >>> Method: %s\n", methodName->chars());
            fprintf(f, "        Arity: %d\n", method->arity);
            fprintf(f, "        Has return: %s\n\n", method->hasReturn ? "yes" : "no");
            
            // Constants
            fprintf(f, "      Constants (%zu):\n", method->chunk->constants.size());
            for (size_t i = 0; i < method->chunk->constants.size(); i++) {
                fprintf(f, "        [%zu] = ", i);
                
                Value v = method->chunk->constants[i];
                if (v.isString()) {
                    fprintf(f, "\"%s\"", v.asString()->chars());
                } else if (v.isInt()) {
                    fprintf(f, "%d", v.asInt());
                } else if (v.isDouble()) {
                    fprintf(f, "%.2f", v.asDouble());
                } else {
                    fprintf(f, "<value>");
                }
                fprintf(f, "\n");
            }
            fprintf(f, "\n");
            
            // Bytecode
            fprintf(f, "      Bytecode:\n");
            for (size_t offset = 0; offset < method->chunk->count;) {
                fprintf(f, "        ");
                
                FILE* old_stdout = stdout;
                stdout = f;
                offset = Debug::disassembleInstruction(*method->chunk, offset);
                stdout = old_stdout;
            }
            fprintf(f, "\n");
        });
        
        fprintf(f, "\n"); });
#endif
}
