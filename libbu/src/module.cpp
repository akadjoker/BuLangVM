#include "compiler.hpp"
#include "code.hpp"
#include "interpreter.hpp"
#include "platform.hpp"
#include "opcode.hpp"
#include "pool.hpp"
#include "value.hpp"
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>

void ModuleDef::clear()
{
    functionNames.destroy();
    constantNames.destroy();
    
}

ModuleDef::ModuleDef(String *name, Interpreter *vm) : name(name), vm(vm)
{
}

uint16 ModuleDef::addFunction(const char *name, NativeFunction func, int arity)
{
    String *nameStr = vm->createString(name);

    uint16 existingId;
     if (functionNames.get(nameStr, &existingId))
     {
         Warning("Function '%s' already exists in module '%s', replacing",name, this->name);
         return existingId;
     }
 
    uint16 id = (uint16)functions.size();

  
    if (id >= 4096)
    {
        Error("Too many functions in module! Max 4096");
        return 0;
    }
    

    functions.push({func, arity});
    functionNames.set(nameStr, id);

    return id;
}

uint16 ModuleDef::addConstant(const char *name, Value value)
{
    String *nameStr = vm->createString(name);

   
    uint16 existingId;
    if (constantNames.get(nameStr, &existingId))
    {
        Warning("Constant '%s' already exists in module '%s', replacing",name, this->name);
        constants[existingId] = value;
        return existingId;
    }

 
    uint16 id = (uint16)constants.size();

    if (id >= 4096)
    {
        Error("Too many constants in module! Max 4096");
        return 0;
    }

    constants.push(value);
    constantNames.set(nameStr, id);

    return id;
}

NativeFunctionDef *ModuleDef::getFunction(uint16 id)
{
    return &functions[id];
}

Value *ModuleDef::getConstant(uint16 id)
{
    return &constants[id];
}

bool ModuleDef::getFunctionId(String *name, uint16 *outId)
{
     return functionNames.get(name, outId);
}

bool ModuleDef::getFunctionId(const char *name, uint16 *outId)
{
    bool found = false;
    
    functionNames.forEachWhile([&](String *key, uint16 value) 
    {
        if (strcmp(key->chars(), name) == 0)
        {
            *outId = value;
            found = true;
            return false;
        }
        return true;
    }
    );
    return found;
}

bool ModuleDef::getConstantId(const char *name, uint16 *outId)
{
    bool found = false;
    
    constantNames.forEachWhile([&](String *key, uint16 value) 
    {
        if (strcmp(key->chars(), name) == 0)
        {
            *outId = value;
            found = true;
            return false;
        }
        return true;
    }
    );
    return found;
}

bool ModuleDef::getConstantId(String *name, uint16 *outId)
{
     return constantNames.get(name, outId);
}

bool ModuleDef::getFunctionName(uint16 id, String **outName)
{
    bool found = false;
    
    functionNames.forEachWhile([&](String *key, uint16 value) 
    {
        if (value == id)
        {
            *outName = key;
            found = true;
            return false;
        }
        return true;
    });
    return found;
   
}

bool ModuleDef::getConstantName(uint16 id, String **outName)
{
        bool found = false;
    
    constantNames.forEachWhile([&](String *key, uint16 value) 
    {
        if (value == id)
        {
            *outName = key;
            found = true;
            return false;
        }
        return true;
    });
    
    return found;
}

ModuleBuilder::ModuleBuilder(ModuleDef *module, Interpreter *vm) : module(module), vm(vm)
{
}

ModuleBuilder &ModuleBuilder::addFunction(const char *name, NativeFunction func, int arity)
{
    module->addFunction(name,func, arity);
    return *this;
}

ModuleBuilder &ModuleBuilder::addInt(const char *name, int value)
{
    module->addConstant(name,vm->makeInt(value));
    return *this;
}

ModuleBuilder &ModuleBuilder::addByte(const char *name, uint8 value)
{
    module->addConstant(name,vm->makeByte(value));
    return *this;
}

ModuleBuilder &ModuleBuilder::addFloat(const char *name, float value)
{
    module->addConstant(name,vm->makeFloat(value));
    return *this;
}

ModuleBuilder &ModuleBuilder::addDouble(const char *name, double value)
{
    module->addConstant(name,vm->makeDouble(value));
    return *this;
}

ModuleBuilder &ModuleBuilder::addBool(const char *name, bool value)
{
    module->addConstant(name,vm->makeBool(value));
    return *this;
}



ModuleBuilder &ModuleBuilder::addString(const char *name, const char *value)
{
    module->addConstant(name,vm->makeString(value));
    return *this;
}

uint16 Interpreter::defineModule(const char *name)
{
    String *nameStr = createString(name);

    uint16 existingId;
    if (moduleNames.get(nameStr, &existingId))
    {
        Warning("Module '%s' already defined, returning existing ID %d",
                name, existingId);
        return existingId;
    }

    ModuleDef *mod = new ModuleDef(nameStr, this);
    uint16 id = (uint16)modules.size();
    totalAllocated += sizeof(ModuleDef);

    if (id >= 4096)
    {
        Error("Too many modules! Max 4096");
        delete mod;
        return 0;
    }

    modules.push(mod);
    moduleNames.set(nameStr, id);
    return id;
}

ModuleBuilder Interpreter::addModule(const char *name)
{
    uint16 id = defineModule(name);

    return ModuleBuilder(modules[id], this);
}

ModuleDef *Interpreter::getModule(uint16 id)
{
    if (id >= modules.size())
        return nullptr;
    return modules[id];
}

bool Interpreter::getModuleId(String *name, uint16 *outId)
{
    return moduleNames.get(name, outId);
}

bool Interpreter::getModuleId(const char *name, uint16 *outId)
{
    bool found = false;
    
    moduleNames.forEachWhile([&](String *key, uint16 value) 
    {
        if (strcmp(key->chars(), name) == 0)
        {
            *outId = value;
            found = true;
            return false;  // Para o loop
        }
        return true;  // Continua
    });
    
    return found;
   
}

bool Interpreter::containsModule(const char *name)
{
    bool found = false;

    moduleNames.forEachWhile([&](String *key, uint16 value)
    {
        if (strcmp(key->chars(), name) == 0)
        {
            found = true;
            return false;  // Para o loop
        }
        return true;  // Continua
    });

    return found;
}

// ============================================
// Plugin System Implementation
// ============================================

#include "plugin.hpp"
#include <cstring>

static void setError(char* dest, size_t destSize, const char* msg)
{
    strncpy(dest, msg, destSize - 1);
    dest[destSize - 1] = '\0';
}

bool Interpreter::loadPlugin(const char *path)
{
    if (loadedPluginCount >= MAX_PLUGINS)
    {
        setError(lastPluginError, sizeof(lastPluginError), "Maximum plugins limit reached");
        return false;
    }

    // Try to load the library
    void* handle = OsLoadLibrary(path);
    if (!handle)
    {
        const char* err = OsGetLibraryError();
        setError(lastPluginError, sizeof(lastPluginError), err ? err : "Unknown error loading library");
        return false;
    }

    // Get the plugin info function
    BuGetPluginInfoFunc getInfo = (BuGetPluginInfoFunc)OsGetSymbol(handle, BU_PLUGIN_SYMBOL);
    if (!getInfo)
    {
        setError(lastPluginError, sizeof(lastPluginError), "Plugin does not export '" BU_PLUGIN_SYMBOL "' function");
        OsFreeLibrary(handle);
        return false;
    }

    // Get plugin info
    BuPluginInfo* info = getInfo();
    if (!info)
    {
        setError(lastPluginError, sizeof(lastPluginError), "Plugin returned null info");
        OsFreeLibrary(handle);
        return false;
    }

    // Check API version
    if (info->apiVersion != BU_PLUGIN_API_VERSION)
    {
        snprintf(lastPluginError, sizeof(lastPluginError),
                 "Plugin API version mismatch: expected %d, got %d",
                 BU_PLUGIN_API_VERSION, info->apiVersion);
        OsFreeLibrary(handle);
        return false;
    }

    // Check if module already exists
    if (containsModule(info->name))
    {
        snprintf(lastPluginError, sizeof(lastPluginError),
                 "Module '%s' already loaded", info->name);
        OsFreeLibrary(handle);
        return false;
    }

    // Register the module
    info->registerModule(this);

    // Track the loaded plugin
    LoadedPlugin& plugin = loadedPlugins[loadedPluginCount++];
    plugin.handle = handle;
    plugin.name = info->name;
    plugin.cleanup = info->cleanup;

    Info("Loaded plugin: %s v%s by %s", info->name, info->version, info->author);
    return true;
}

bool Interpreter::loadPluginByName(const char *name)
{
    char filename[MAX_PATH_LEN];
    char fullPath[MAX_PATH_LEN];

    // Build filename: libbu_<name>.so (or .dll/.dylib)
    snprintf(filename, sizeof(filename), "libbu_%s%s", name, OsGetLibraryExtension());

    // Try current directory first
    if (loadPlugin(filename))
        return true;

    // Try ./modules/
    snprintf(fullPath, sizeof(fullPath), "modules/%s", filename);
    if (loadPlugin(fullPath))
        return true;

    // Try each search path
    for (int i = 0; i < pluginSearchPathCount; i++)
    {
        const char* basePath = pluginSearchPaths[i];
        size_t len = strlen(basePath);

        if (len > 0 && basePath[len - 1] != '/' && basePath[len - 1] != '\\')
            snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, filename);
        else
            snprintf(fullPath, sizeof(fullPath), "%s%s", basePath, filename);

        if (loadPlugin(fullPath))
            return true;
    }

    snprintf(lastPluginError, sizeof(lastPluginError),
             "Could not find plugin '%s' (%s)", name, filename);
    return false;
}

void Interpreter::addPluginSearchPath(const char *path)
{
    if (pluginSearchPathCount >= MAX_PLUGIN_PATHS)
    {
        Warning("Maximum plugin search paths reached");
        return;
    }

    strncpy(pluginSearchPaths[pluginSearchPathCount], path, MAX_PATH_LEN - 1);
    pluginSearchPaths[pluginSearchPathCount][MAX_PATH_LEN - 1] = '\0';
    pluginSearchPathCount++;
}

void Interpreter::unloadAllPlugins()
{
    // Call cleanup functions and free libraries in reverse order
    for (int i = loadedPluginCount - 1; i >= 0; i--)
    {
        LoadedPlugin& plugin = loadedPlugins[i];
        if (plugin.cleanup)
        {
            plugin.cleanup();
        }
        OsFreeLibrary(plugin.handle);
    }
    loadedPluginCount = 0;
}

const char* Interpreter::getLastPluginError() const
{
    return lastPluginError;
}