#include "webapi.h"
#include <emscripten.h>
#include <emscripten/em_asm.h>
#include <string>

// ============================================
// DOM Manipulation
// ============================================

EM_JS(char *, js_dom_create, (const char *tag_cstr), {
    const tag = UTF8ToString(tag_cstr);
    const elem = document.createElement(tag);
    const id = 'elem_' + Math.random().toString(36).substr(2, 9);
    elem.id = id;

    const len = lengthBytesUTF8(id) + 1;
    const ptr = _malloc(len);
    stringToUTF8(id, ptr, len);
    return ptr;
});

EM_JS(int, js_dom_append, (const char *parent_cstr, const char *child_cstr), {
    const parentId = UTF8ToString(parent_cstr);
    const childId = UTF8ToString(child_cstr);

    const parent = document.getElementById(parentId);
    const child = document.getElementById(childId);

    if (!parent || !child)
        return 0;
    parent.appendChild(child);
    return 1;
});

EM_JS(int, js_dom_remove, (const char *id_cstr), {
    const id = UTF8ToString(id_cstr);
    const elem = document.getElementById(id);
    if (!elem)
        return 0;
    elem.remove();
    return 1;
});

EM_JS(int, js_dom_set_attr, (const char *id_cstr, const char *attr_cstr, const char *value_cstr), {
    const id = UTF8ToString(id_cstr);
    const attr = UTF8ToString(attr_cstr);
    const value = UTF8ToString(value_cstr);

    const elem = document.getElementById(id);
    if (!elem)
        return 0;
    elem.setAttribute(attr, value);
    return 1;
});

EM_JS(char*, js_dom_get_attr, (const char *id_cstr, const char *attr_cstr), {
    const id = UTF8ToString(id_cstr);
    const attr = UTF8ToString(attr_cstr);
    
    const elem = document.getElementById(id);
    if (!elem) return 0;
    
    const value = elem.getAttribute(attr);
    if (value === null) return 0;  
    
    const len = lengthBytesUTF8(value) + 1;
    const ptr = _malloc(len);
    stringToUTF8(value, ptr, len);
    return ptr;
});


EM_JS(int, js_dom_remove_attr, (const char *id_cstr, const char *attr_cstr), {
    const id = UTF8ToString(id_cstr);
    const attr = UTF8ToString(attr_cstr);

    const elem = document.getElementById(id);
    if (!elem)
        return 0;
    elem.removeAttribute(attr);
    return 1;
});

EM_JS(int, js_canvas_set_stroke_style, (const char *id_cstr, const char *css_cstr), {
    const id = UTF8ToString(id_cstr);
    const css = UTF8ToString(css_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.strokeStyle = css;
    return 1;
});

EM_JS(int, js_canvas_set_line_width, (const char *id_cstr, double width), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.lineWidth = width;
    return 1;
});

EM_JS(int, js_canvas_stroke_rect, (const char *id_cstr, double x, double y, double w, double h), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.strokeRect(x, y, w, h);
    return 1;
});

EM_JS(int, js_canvas_begin_path, (const char *id_cstr), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.beginPath();
    return 1;
});

EM_JS(int, js_canvas_move_to, (const char *id_cstr, double x, double y), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.moveTo(x, y);
    return 1;
});

EM_JS(int, js_canvas_line_to, (const char *id_cstr, double x, double y), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.lineTo(x, y);
    return 1;
});

EM_JS(int, js_canvas_stroke, (const char *id_cstr), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.stroke();
    return 1;
});

EM_JS(int, js_canvas_arc, (const char *id_cstr, double x, double y, double radius, double startAngle, double endAngle), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.arc(x, y, radius, startAngle, endAngle);
    return 1;
});

EM_JS(int, js_canvas_create, (const char *id_cstr, int w, int h), {
    const id = UTF8ToString(id_cstr);
    let c = document.getElementById(id);
    if (!c)
    {
        c = document.createElement('canvas');
        c.id = id;
        document.body.appendChild(c);
    }
    c.width = w;
    c.height = h;
    return 1;
});

EM_JS(int, js_canvas_clear, (const char *id_cstr), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.clearRect(0, 0, c.width, c.height);
    return 1;
});

EM_JS(int, js_canvas_set_fill_style, (const char *id_cstr, const char *css_cstr), {
    const id = UTF8ToString(id_cstr);
    const css = UTF8ToString(css_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.fillStyle = css;
    return 1;
});

EM_JS(int, js_canvas_fill_rect, (const char *id_cstr, double x, double y, double w, double h), {
    const id = UTF8ToString(id_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.fillRect(x, y, w, h);
    return 1;
});

EM_JS(int, js_canvas_set_font, (const char *id_cstr, const char *font_cstr), {
    const id = UTF8ToString(id_cstr);
    const font = UTF8ToString(font_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.font = font;
    return 1;
});

EM_JS(int, js_canvas_fill_text, (const char *id_cstr, const char *text_cstr, double x, double y), {
    const id = UTF8ToString(id_cstr);
    const text = UTF8ToString(text_cstr);
    const c = document.getElementById(id);
    if (!c)
        return 0;
    const ctx = c.getContext('2d');
    ctx.fillText(text, x, y);
    return 1;
});

// ============================================
// Helper: Value to String
// ============================================
static std::string valueToString(const Value &v)
{
    char buffer[256];

    switch (v.type)
    {
    case ValueType::NIL:
        return "nil";
    case ValueType::BOOL:
        return v.as.boolean ? "true" : "false";
    case ValueType::INT:
        snprintf(buffer, sizeof(buffer), "%d", v.as.integer);
        return buffer;
    case ValueType::DOUBLE:
        snprintf(buffer, sizeof(buffer), "%.2f", v.as.number);
        return buffer;
    case ValueType::STRING:
        return v.as.string->chars();
    default:
        return "<object>";
    }
}

// ============================================
// DOM Manipulation
// ============================================

Value native_dom_set_text(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2)
    {
        vm->runtimeError("dom_set_text expects 2 arguments: (id, text)");
        return vm->makeNil();
    }

    if (args[0].type != ValueType::STRING)
    {
        vm->runtimeError("dom_set_text: id must be string");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    std::string text = valueToString(args[1]);

    // JavaScript inline
    EM_ASM({
        var element = document.getElementById(UTF8ToString($0));
        if (element) {
            element.textContent = UTF8ToString($1);
        } }, id, text.c_str());

    return vm->makeNil();
}

Value native_dom_set_html(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2)
    {
        vm->runtimeError("dom_set_html expects 2 arguments");
        return vm->makeNil();
    }

    if (args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("dom_set_html: both arguments must be strings");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    const char *html = args[1].as.string->chars();

    EM_ASM({
        var element = document.getElementById(UTF8ToString($0));
        if (element) {
            element.innerHTML = UTF8ToString($1);
        } }, id, html);

    return vm->makeNil();
}

Value native_dom_get_value(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("dom_get_value expects string id");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();

    char *result = (char *)EM_ASM_INT({
        var element = document.getElementById(UTF8ToString($0));
        if (element && element.value !== undefined) {
            var str = element.value;
            var len = lengthBytesUTF8(str) + 1;
            var ptr = _malloc(len);
            stringToUTF8(str, ptr, len);
            return ptr;
        }
        return 0; }, id);

    if (result)
    {
        Value v = vm->makeString(result);
        free(result);
        return v;
    }

    return vm->makeNil();
}

Value native_dom_set_value(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2)
    {
        vm->runtimeError("dom_set_value expects 2 arguments");
        return vm->makeNil();
    }

    if (args[0].type != ValueType::STRING)
    {
        vm->runtimeError("dom_set_value: id must be string");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    std::string value = valueToString(args[1]);

    EM_ASM({
        var element = document.getElementById(UTF8ToString($0));
        if (element && element.value !== undefined) {
            element.value = UTF8ToString($1);
        } }, id, value.c_str());

    return vm->makeNil();
}

// ============================================
// Console
// ============================================

Value native_console_log(Interpreter *vm, int argCount, Value *args)
{
    std::string output;

    for (int i = 0; i < argCount; i++)
    {
        output += valueToString(args[i]);
        if (i < argCount - 1)
            output += " ";
    }

    EM_ASM({ console.log(UTF8ToString($0)); }, output.c_str());

    return vm->makeNil();
}

Value native_console_error(Interpreter *vm, int argCount, Value *args)
{
    std::string output;

    for (int i = 0; i < argCount; i++)
    {
        output += valueToString(args[i]);
        if (i < argCount - 1)
            output += " ";
    }

    EM_ASM({ console.error(UTF8ToString($0)); }, output.c_str());

    return vm->makeNil();
}

Value native_console_warn(Interpreter *vm, int argCount, Value *args)
{
    std::string output;

    for (int i = 0; i < argCount; i++)
    {
        output += valueToString(args[i]);
        if (i < argCount - 1)
            output += " ";
    }

    EM_ASM({ console.warn(UTF8ToString($0)); }, output.c_str());

    return vm->makeNil();
}

// ============================================
// LocalStorage
// ============================================

// Value native_storage_set(Interpreter *vm, int argCount, Value *args)
// {
//     if (argCount < 2 || args[0].type != ValueType::STRING)
//     {
//         vm->runtimeError("storage_set expects (key, value)");
//         return vm->makeNil();
//     }

//     const char *key = args[0].as.string->chars();
//     std::string value = valueToString(args[1]);

//     EM_ASM({ localStorage.setItem(UTF8ToString($0), UTF8ToString($1)); }, key, value.c_str());

//     return vm->makeNil();
// }

// Value native_storage_get(Interpreter *vm, int argCount, Value *args)
// {
//     if (argCount < 1 || args[0].type != ValueType::STRING)
//     {
//         vm->runtimeError("storage_get expects string key");
//         return vm->makeNil();
//     }

//     const char *key = args[0].as.string->chars();

//     char *result = (char *)EM_ASM_INT({
//         var value = localStorage.getItem(UTF8ToString($0));
//         if (value !== null) {
//             var len = lengthBytesUTF8(value) + 1;
//             var ptr = _malloc(len);
//             stringToUTF8(value, ptr, len);
//             return ptr;
//         }
//         return 0; }, key);

//     if (result)
//     {
//         Value v = vm->makeString(result);
//         free(result);
//         return v;
//     }

//     return vm->makeNil();
// }

// Value native_storage_remove(Interpreter *vm, int argCount, Value *args)
// {
//     if (argCount < 1 || args[0].type != ValueType::STRING)
//     {
//         vm->runtimeError("storage_remove expects string key");
//         return vm->makeNil();
//     }

//     const char *key = args[0].as.string->chars();

//     EM_ASM({ localStorage.removeItem(UTF8ToString($0)); }, key);

//     return vm->makeNil();
// }

// ============================================
// Alert/Prompt/Confirm
// ============================================

Value native_alert(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1)
    {
        vm->runtimeError("alert expects 1 argument");
        return vm->makeNil();
    }

    std::string message = valueToString(args[0]);

    EM_ASM({ alert(UTF8ToString($0)); }, message.c_str());

    return vm->makeNil();
}

Value native_prompt(Interpreter *vm, int argCount, Value *args)
{
    const char *message = argCount > 0 && args[0].type == ValueType::STRING
                              ? args[0].as.string->chars()
                              : "Enter value:";

    const char *defaultValue = argCount > 1 && args[1].type == ValueType::STRING
                                   ? args[1].as.string->chars()
                                   : "";

    char *result = (char *)EM_ASM_INT({
        var value = prompt(UTF8ToString($0), UTF8ToString($1));
        if (value !== null) {
            var len = lengthBytesUTF8(value) + 1;
            var ptr = _malloc(len);
            stringToUTF8(value, ptr, len);
            return ptr;
        }
        return 0; }, message, defaultValue);

    if (result)
    {
        Value v = vm->makeString(result);
        free(result);
        return v;
    }

    return vm->makeNil();
}

Value native_confirm(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1)
    {
        vm->runtimeError("confirm expects 1 argument");
        return vm->makeNil();
    }

    std::string message = valueToString(args[0]);

    bool result = EM_ASM_INT({ return confirm(UTF8ToString($0)) ? 1 : 0; }, message.c_str());

    return vm->makeBool(result);
}

Value native_canvas_in_output(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2)
    {
        vm->runtimeError("canvas_in_output expects (width, height)");
        return vm->makeNil();
    }

    int w = (args[0].type == ValueType::INT)      ? args[0].as.integer
            : (args[0].type == ValueType::DOUBLE) ? (int)args[0].as.number
                                                  : 0;

    int h = (args[1].type == ValueType::INT)      ? args[1].as.integer
            : (args[1].type == ValueType::DOUBLE) ? (int)args[1].as.number
                                                  : 0;

    if (w <= 0 || h <= 0)
    {
        vm->runtimeError("canvas_in_output: width/height must be > 0");
        return vm->makeNil();
    }

    EM_ASM({
        var out = document.getElementById("output");
        if (!out) return;

        // Mete um canvas dentro do output
        out.innerHTML =
          '<canvas id="__canvas__" width="' + $0 + '" height="' + $1 + '" ' +
          'style="display:block; width:100%; height:auto; background:#111; border-radius:8px;"></canvas>'; }, w, h);

    return vm->makeString("__canvas__"); // devolve o id do canvas
}

Value native_canvas_create(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 3)
    {
        vm->runtimeError("canvas_create expects (id, width, height)");
        return vm->makeNil();
    }
    if (args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_create: id must be string");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    int w = (args[1].type == ValueType::INT)      ? args[1].as.integer
            : (args[1].type == ValueType::DOUBLE) ? (int)args[1].as.number
                                                  : 0;
    int h = (args[2].type == ValueType::INT)      ? args[2].as.integer
            : (args[2].type == ValueType::DOUBLE) ? (int)args[2].as.number
                                                  : 0;

    if (w <= 0 || h <= 0)
    {
        vm->runtimeError("canvas_create: width/height must be > 0");
        return vm->makeNil();
    }

    int ok = js_canvas_create(id, w, h);
    return vm->makeBool(ok != 0);
}

Value native_canvas_clear(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_clear expects (id)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    int ok = js_canvas_clear(id);
    return vm->makeBool(ok != 0);
}

Value native_canvas_set_fill_style(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_set_fill_style expects (id, cssColor)");
        return vm->makeNil();
    }
    if (args[1].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_set_fill_style: cssColor must be string (e.g. '#ff0' or 'red')");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    const char *css = args[1].as.string->chars();
    int ok = js_canvas_set_fill_style(id, css);
    return vm->makeBool(ok != 0);
}

Value native_canvas_fill_rect(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 5 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_fill_rect expects (id, x, y, w, h)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    double w = (args[3].type == ValueType::INT) ? (double)args[3].as.integer : args[3].as.number;
    double h = (args[4].type == ValueType::INT) ? (double)args[4].as.integer : args[4].as.number;

    int ok = js_canvas_fill_rect(id, x, y, w, h);
    return vm->makeBool(ok != 0);
}

Value native_canvas_set_font(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_set_font expects (id, fontString) e.g. '16px sans-serif'");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    const char *font = args[1].as.string->chars();
    int ok = js_canvas_set_font(id, font);
    return vm->makeBool(ok != 0);
}

Value native_canvas_fill_text(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 4 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_fill_text expects (id, text, x, y)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    std::string text = valueToString(args[1]);
    double x = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    double y = (args[3].type == ValueType::INT) ? (double)args[3].as.integer : args[3].as.number;

    int ok = js_canvas_fill_text(id, text.c_str(), x, y);
    return vm->makeBool(ok != 0);
}

Value native_canvas_set_stroke_style(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_set_stroke_style expects (id, cssColor)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    const char *css = args[1].as.string->chars();
    int ok = js_canvas_set_stroke_style(id, css);
    return vm->makeBool(ok != 0);
}

Value native_canvas_set_line_width(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_set_line_width expects (id, width)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double width = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    int ok = js_canvas_set_line_width(id, width);
    return vm->makeBool(ok != 0);
}

Value native_canvas_stroke_rect(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 5 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_stroke_rect expects (id, x, y, w, h)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    double w = (args[3].type == ValueType::INT) ? (double)args[3].as.integer : args[3].as.number;
    double h = (args[4].type == ValueType::INT) ? (double)args[4].as.integer : args[4].as.number;
    int ok = js_canvas_stroke_rect(id, x, y, w, h);
    return vm->makeBool(ok != 0);
}

Value native_canvas_begin_path(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_begin_path expects (id)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    int ok = js_canvas_begin_path(id);
    return vm->makeBool(ok != 0);
}

Value native_canvas_move_to(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 3 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_move_to expects (id, x, y)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    int ok = js_canvas_move_to(id, x, y);
    return vm->makeBool(ok != 0);
}

Value native_canvas_line_to(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 3 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_line_to expects (id, x, y)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    int ok = js_canvas_line_to(id, x, y);
    return vm->makeBool(ok != 0);
}

Value native_canvas_stroke(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_stroke expects (id)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    int ok = js_canvas_stroke(id);
    return vm->makeBool(ok != 0);
}

Value native_canvas_arc(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 6 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_arc expects (id, x, y, radius, startAngle, endAngle)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    double radius = (args[3].type == ValueType::INT) ? (double)args[3].as.integer : args[3].as.number;
    double startAngle = (args[4].type == ValueType::INT) ? (double)args[4].as.integer : args[4].as.number;
    double endAngle = (args[5].type == ValueType::INT) ? (double)args[5].as.integer : args[5].as.number;
    int ok = js_canvas_arc(id, x, y, radius, startAngle, endAngle);
    return vm->makeBool(ok != 0);
}

Value native_canvas_fill_circle(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 4 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_fill_circle expects (id, x, y, radius)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    double radius = (args[3].type == ValueType::INT) ? (double)args[3].as.integer : args[3].as.number;

    js_canvas_begin_path(id);
    js_canvas_arc(id, x, y, radius, 0, 6.28318530718); // 2*PI
    EM_ASM({
        const c = document.getElementById(UTF8ToString($0));
        if (c) c.getContext('2d').fill(); }, id);

    return vm->makeBool(true);
}

Value native_canvas_stroke_circle(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 4 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("canvas_stroke_circle expects (id, x, y, radius)");
        return vm->makeNil();
    }
    const char *id = args[0].as.string->chars();
    double x = (args[1].type == ValueType::INT) ? (double)args[1].as.integer : args[1].as.number;
    double y = (args[2].type == ValueType::INT) ? (double)args[2].as.integer : args[2].as.number;
    double radius = (args[3].type == ValueType::INT) ? (double)args[3].as.integer : args[3].as.number;

    js_canvas_begin_path(id);
    js_canvas_arc(id, x, y, radius, 0, 6.28318530718); // 2*PI
    js_canvas_stroke(id);

    return vm->makeBool(true);
}

Value native_dom_create(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("dom_create expects (tagName) e.g. 'div'");
        return vm->makeNil();
    }

    const char *tag = args[0].as.string->chars();
    char *id = js_dom_create(tag);

    if (id)
    {
        Value v = vm->makeString(id);
        free(id);
        return v;
    }

    return vm->makeNil();
}

Value native_dom_append(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("dom_append expects (parentId, childId)");
        return vm->makeNil();
    }

    const char *parent = args[0].as.string->chars();
    const char *child = args[1].as.string->chars();
    int ok = js_dom_append(parent, child);
    return vm->makeBool(ok != 0);
}

Value native_dom_remove(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("dom_remove expects (id)");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    int ok = js_dom_remove(id);
    return vm->makeBool(ok != 0);
}

Value native_dom_set_attr(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 3 || args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("dom_set_attr expects (id, attr, value)");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    const char *attr = args[1].as.string->chars();
    std::string value = valueToString(args[2]);

    int ok = js_dom_set_attr(id, attr, value.c_str());
    return vm->makeBool(ok != 0);
}

Value native_dom_get_attr(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("dom_get_attr expects (id, attr)");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    const char *attr = args[1].as.string->chars();

    char *value = js_dom_get_attr(id, attr);
    if (value)
    {
        Value v = vm->makeString(value);
        free(value);
        return v;
    }

    return vm->makeNil();
}

Value native_dom_remove_attr(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || args[0].type != ValueType::STRING || args[1].type != ValueType::STRING)
    {
        vm->runtimeError("dom_remove_attr expects (id, attr)");
        return vm->makeNil();
    }

    const char *id = args[0].as.string->chars();
    const char *attr = args[1].as.string->chars();
    int ok = js_dom_remove_attr(id, attr);
    return vm->makeBool(ok != 0);
}

// ============================================
// Register All Web Natives
// ============================================

void registerWebNatives(Interpreter *vm)
{
    // DOM
    vm->registerNative("dom_set_text", native_dom_set_text, 2);
    vm->registerNative("dom_set_html", native_dom_set_html, 2);
    vm->registerNative("dom_get_value", native_dom_get_value, 1);
    vm->registerNative("dom_set_value", native_dom_set_value, 2);
    vm->registerNative("dom_create", native_dom_create, 1);
    vm->registerNative("dom_append", native_dom_append, 2);
    vm->registerNative("dom_remove", native_dom_remove, 1);
    vm->registerNative("dom_set_attr", native_dom_set_attr, 3);
    vm->registerNative("dom_get_attr", native_dom_get_attr, 2);
    vm->registerNative("dom_remove_attr", native_dom_remove_attr, 2);

    // Console
    vm->registerNative("console_log", native_console_log, -1);
    vm->registerNative("console_error", native_console_error, -1);
    vm->registerNative("console_warn", native_console_warn, -1);

    // Storage
    // vm->registerNative("storage_set", native_storage_set, 2);
    // vm->registerNative("storage_get", native_storage_get, 1);
    // vm->registerNative("storage_remove", native_storage_remove, 1);

    // Dialogs
    vm->registerNative("alert", native_alert, 1);
    vm->registerNative("prompt", native_prompt, -1);
    vm->registerNative("confirm", native_confirm, 1);

    vm->registerNative("canvas_in_output", native_canvas_in_output, 2);

    vm->registerNative("canvas_create", native_canvas_create, 3);
    vm->registerNative("canvas_clear", native_canvas_clear, 1);
    vm->registerNative("canvas_set_fill_style", native_canvas_set_fill_style, 2);
    vm->registerNative("canvas_fill_rect", native_canvas_fill_rect, 5);
    vm->registerNative("canvas_set_font", native_canvas_set_font, 2);
    vm->registerNative("canvas_fill_text", native_canvas_fill_text, 4);
    vm->registerNative("canvas_set_stroke_style", native_canvas_set_stroke_style, 2);
    vm->registerNative("canvas_set_line_width", native_canvas_set_line_width, 2);
    vm->registerNative("canvas_stroke_rect", native_canvas_stroke_rect, 5);
    vm->registerNative("canvas_begin_path", native_canvas_begin_path, 1);
    vm->registerNative("canvas_move_to", native_canvas_move_to, 3);
    vm->registerNative("canvas_line_to", native_canvas_line_to, 3);
    vm->registerNative("canvas_stroke", native_canvas_stroke, 1);
    vm->registerNative("canvas_arc", native_canvas_arc, 6);
    vm->registerNative("canvas_fill_circle", native_canvas_fill_circle, 4);
    vm->registerNative("canvas_stroke_circle", native_canvas_stroke_circle, 4);
}
