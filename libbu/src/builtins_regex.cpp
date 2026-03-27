#include "interpreter.hpp"

#ifdef BU_ENABLE_REGEX

#include <regex>
#include <string>

int native_regex_match(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isString() || !args[1].isString())
    {
        vm->runtimeError("regex.match expects (pattern, text)");
        return 0;
    }

    try
    {
        const std::regex re(args[0].asStringChars());
        const bool matched = std::regex_match(args[1].asStringChars(), re);
        vm->push(vm->makeBool(matched));
        return 1;
    }
    catch (const std::regex_error &e)
    {
        vm->runtimeError("regex.match invalid pattern: %s", e.what());
        return 0;
    }
}

int native_regex_search(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isString() || !args[1].isString())
    {
        vm->runtimeError("regex.search expects (pattern, text)");
        return 0;
    }

    try
    {
        const std::regex re(args[0].asStringChars());
        const std::string text = args[1].asStringChars();
        std::smatch match;

        if (!std::regex_search(text, match, re))
        {
            vm->push(vm->makeNil());
            return 1;
        }

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();

        map->table.set(vm->makeString("match"), vm->makeString(match[0].str().c_str()));
        map->table.set(vm->makeString("index"), vm->makeInt((int)match.position(0)));

        Value groups = vm->makeArray();
        ArrayInstance *arr = groups.asArray();
        for (size_t i = 1; i < match.size(); ++i)
        {
            if (match[i].matched)
                arr->values.push(vm->makeString(match[i].str().c_str()));
            else
                arr->values.push(vm->makeNil());
        }
        map->table.set(vm->makeString("groups"), groups);

        vm->push(result);
        return 1;
    }
    catch (const std::regex_error &e)
    {
        vm->runtimeError("regex.search invalid pattern: %s", e.what());
        return 0;
    }
}

int native_regex_replace(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 3 || !args[0].isString() || !args[1].isString() || !args[2].isString())
    {
        vm->runtimeError("regex.replace expects (pattern, replacement, text)");
        return 0;
    }

    try
    {
        const std::regex re(args[0].asStringChars());
        const std::string replaced = std::regex_replace(
            std::string(args[2].asStringChars()),
            re,
            std::string(args[1].asStringChars()));
        vm->push(vm->makeString(replaced.c_str()));
        return 1;
    }
    catch (const std::regex_error &e)
    {
        vm->runtimeError("regex.replace invalid pattern: %s", e.what());
        return 0;
    }
}

int native_regex_findall(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isString() || !args[1].isString())
    {
        vm->runtimeError("regex.findall expects (pattern, text)");
        return 0;
    }

    try
    {
        const std::regex re(args[0].asStringChars());
        const std::string text = args[1].asStringChars();

        Value out = vm->makeArray();
        ArrayInstance *arr = out.asArray();

        std::sregex_iterator it(text.begin(), text.end(), re);
        std::sregex_iterator end;

        // Detect number of capture groups from first match
        bool hasGroups = false;
        size_t groupCount = 0;
        if (it != end)
        {
            groupCount = it->size() - 1; // size() includes full match [0]
            hasGroups = (groupCount > 0);
        }

        for (; it != end; ++it)
        {
            if (!hasGroups)
            {
                // No groups: return array of full matches
                arr->values.push(vm->makeString(it->str().c_str()));
            }
            else if (groupCount == 1)
            {
                // Single group: return array of group strings
                if ((*it)[1].matched)
                    arr->values.push(vm->makeString((*it)[1].str().c_str()));
                else
                    arr->values.push(vm->makeNil());
            }
            else
            {
                // Multiple groups: return array of arrays
                Value sub = vm->makeArray();
                ArrayInstance *subArr = sub.asArray();
                for (size_t g = 1; g <= groupCount; ++g)
                {
                    if ((*it)[g].matched)
                        subArr->values.push(vm->makeString((*it)[g].str().c_str()));
                    else
                        subArr->values.push(vm->makeNil());
                }
                arr->values.push(sub);
            }
        }

        vm->push(out);
        return 1;
    }
    catch (const std::regex_error &e)
    {
        vm->runtimeError("regex.findall invalid pattern: %s", e.what());
        return 0;
    }
}

int native_regex_split(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isString() || !args[1].isString())
    {
        vm->runtimeError("regex.split expects (pattern, text)");
        return 0;
    }

    try
    {
        const std::regex re(args[0].asStringChars());
        const std::string text = args[1].asStringChars();

        Value out = vm->makeArray();
        ArrayInstance *arr = out.asArray();

        std::sregex_token_iterator it(text.begin(), text.end(), re, -1);
        std::sregex_token_iterator end;
        for (; it != end; ++it)
        {
            arr->values.push(vm->makeString(it->str().c_str()));
        }

        vm->push(out);
        return 1;
    }
    catch (const std::regex_error &e)
    {
        vm->runtimeError("regex.split invalid pattern: %s", e.what());
        return 0;
    }
}

void Interpreter::registerRegex()
{
    addModule("regex")
        .addFunction("match", native_regex_match, 2)
        .addFunction("search", native_regex_search, 2)
        .addFunction("replace", native_regex_replace, 3)
        .addFunction("findall", native_regex_findall, 2)
        .addFunction("split", native_regex_split, 2);
}

#endif
