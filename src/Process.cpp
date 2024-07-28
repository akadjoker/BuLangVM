
#include "pch.h"

#include "Config.hpp"
#include "Vm.hpp"

extern void printValue(const Value &v);
extern void debugValue(const Value &v);
extern void printValueln(const Value &v);





Process::Process(VirtualMachine *vm,   const char *name):Task(vm,name)
{
    type = ObjectType::OPROCESS;

    isCreated = false;
    frames = 0;
    instance.ID = ID;
    instance.name = name;
    instance.user_data = nullptr;
    x = NUMBER(0);
    y = NUMBER(0);
    graph = NUMBER(0);
    local.define("x", std::move(x));
    local.define("y", std::move(y));
    local.define("graph", std::move(graph));
    mark();

}

Process::~Process()
{
  //INFO("Destroy process: %s", name.c_str());

}

void Process::create()
{
    if (isCreated) return;
    local.read("x", x);
    local.read("y", y);
    local.read("graph", graph);


    instance.locals[IX]     = static_cast<s32>(AS_NUMBER(x));
    instance.locals[IY]     = static_cast<s32>(AS_NUMBER(y));
    instance.locals[IGRAPH] = static_cast<s32>(AS_NUMBER(graph));

    


    isCreated = true;
  
    vm->hooks.instance_create_hook(&instance);
    
}

void Process::update()
{
    if (!isCreated) 
    {
        frames++;
    }
    local.read("x", x);
    local.read("y", y);
    local.read("graph", graph);


    instance.locals[IX]     = static_cast<s32>(AS_NUMBER(x));
    instance.locals[IY]     = static_cast<s32>(AS_NUMBER(y));
    instance.locals[IGRAPH] = static_cast<s32>(AS_NUMBER(graph));
    vm->hooks.instance_pre_execute_hook(&instance);


   //printf("Process: %d\n", frames);
   if (frames > 1 && !isCreated)
   {
        create();
        return;
   }

    if (!isCreated) return;


    




    vm->hooks.process_exec_hook(&instance);




    x = NUMBER(static_cast<double>(instance.locals[IX])); 
    y = NUMBER(static_cast<double>(instance.locals[IY]));
    graph = NUMBER(static_cast<double>(instance.locals[IGRAPH]));

    local.write("x", std::move(x));
    local.write("y", std::move(y));
    local.write("graph", std::move(graph));


    vm->hooks.instance_pos_execute_hook(&instance);
   
   
}

void Process::remove()
{
    Done();
    unmark();

    instance.locals[IX]     = static_cast<s32>(AS_NUMBER(x));
    instance.locals[IY]     = static_cast<s32>(AS_NUMBER(y));
    instance.locals[IGRAPH] = static_cast<s32>(AS_NUMBER(graph));
    vm->hooks.instance_destroy_hook(&instance);
}

void default_instance_create_hook(Instance *instance)
{
   // INFO("Create instance: %s", instance->name.c_str());
}
void default_instance_destroy_hook(Instance *instance)
{
    //INFO("Destroy instance: %s", instance->name.c_str());
}
void default_instance_pre_execute_hook(Instance *instance)
{
   // INFO("Pre execute instance: %s", instance->name.c_str());
}
void default_instance_pos_execute_hook(Instance *instance)
{
   // INFO("Pos execute instance: %s", instance->name.c_str());
}
void default_process_exec_hook(Instance *instance)
{
   // INFO("Process exec instance: %s", instance->name.c_str());
}
