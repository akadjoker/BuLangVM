#include "bindings.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

namespace SDLBindings
{
    // =====================================================
    // SDL_GL CONTEXT FUNCTIONS
    // =====================================================

    Value native_SDL_GL_CreateContext(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_GL_CreateContext expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_GL_CreateContext expects window pointer");
            return vm->makeNil();
        }

        SDL_Window *window = (SDL_Window *)args[0].asPointer();
        SDL_GLContext context = SDL_GL_CreateContext(window);
        if (context)
            return vm->makePointer(context);
        return vm->makeNil();
    }

    Value native_SDL_GL_DeleteContext(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_GL_DeleteContext expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_GL_DeleteContext expects context pointer");
            return vm->makeNil();
        }

        SDL_GLContext context = (SDL_GLContext)args[0].asPointer();
        SDL_GL_DeleteContext(context);
        return vm->makeNil();
    }

    Value native_SDL_GL_MakeCurrent(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SDL_GL_MakeCurrent expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer() || !args[1].isPointer())
        {
            Error("SDL_GL_MakeCurrent expects window and context pointers");
            return vm->makeNil();
        }

        SDL_Window *window = (SDL_Window *)args[0].asPointer();
        SDL_GLContext context = (SDL_GLContext)args[1].asPointer();
        int result = SDL_GL_MakeCurrent(window, context);
        return vm->makeInt(result);
    }

    Value native_SDL_GL_SwapWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_GL_SwapWindow expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_GL_SwapWindow expects window pointer");
            return vm->makeNil();
        }

        SDL_Window *window = (SDL_Window *)args[0].asPointer();
        SDL_GL_SwapWindow(window);
        return vm->makeNil();
    }

    Value native_SDL_GL_SetAttribute(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SDL_GL_SetAttribute expects 2 arguments");
            return vm->makeNil();
        }

        SDL_GLattr attr = (SDL_GLattr)args[0].asNumber();
        int value = args[1].asNumber();
        int result = SDL_GL_SetAttribute(attr, value);
        return vm->makeInt(result);
    }

    Value native_SDL_GL_GetAttribute(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_GL_GetAttribute expects 1 argument");
            return vm->makeNil();
        }

        SDL_GLattr attr = (SDL_GLattr)args[0].asNumber();
        int value;
        int result = SDL_GL_GetAttribute(attr, &value);
        if (result == 0)
            return vm->makeInt(value);
        return vm->makeNil();
    }

    Value native_SDL_GL_SetSwapInterval(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_GL_SetSwapInterval expects 1 argument");
            return vm->makeNil();
        }

        int interval = args[0].asNumber();
        int result = SDL_GL_SetSwapInterval(interval);
        return vm->makeInt(result);
    }

    Value native_SDL_GL_GetSwapInterval(Interpreter *vm, int argc, Value *args)
    {
        int interval = SDL_GL_GetSwapInterval();
        return vm->makeInt(interval);
    }

    Value native_SDL_GL_GetCurrentContext(Interpreter *vm, int argc, Value *args)
    {
        SDL_GLContext context = SDL_GL_GetCurrentContext();
        if (context)
            return vm->makePointer(context);
        return vm->makeNil();
    }

    Value native_SDL_GL_GetCurrentWindow(Interpreter *vm, int argc, Value *args)
    {
        SDL_Window *window = SDL_GL_GetCurrentWindow();
        if (window)
            return vm->makePointer(window);
        return vm->makeNil();
    }

    // =====================================================
    // BASIC OPENGL FUNCTIONS
    // =====================================================

    Value native_glClearColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("glClearColor expects 4 arguments (r, g, b, a)");
            return vm->makeNil();
        }

        GLfloat r = args[0].asNumber();
        GLfloat g = args[1].asNumber();
        GLfloat b = args[2].asNumber();
        GLfloat a = args[3].asNumber();
        glClearColor(r, g, b, a);
        return vm->makeNil();
    }

    Value native_glClear(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glClear expects 1 argument");
            return vm->makeNil();
        }

        GLbitfield mask = args[0].asNumber();
        glClear(mask);
        return vm->makeNil();
    }

    Value native_glViewport(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("glViewport expects 4 arguments (x, y, width, height)");
            return vm->makeNil();
        }

        GLint x = args[0].asNumber();
        GLint y = args[1].asNumber();
        GLsizei width = args[2].asNumber();
        GLsizei height = args[3].asNumber();
        glViewport(x, y, width, height);
        return vm->makeNil();
    }

    Value native_glEnable(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glEnable expects 1 argument");
            return vm->makeNil();
        }

        GLenum cap = args[0].asNumber();
        glEnable(cap);
        return vm->makeNil();
    }

    Value native_glDisable(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glDisable expects 1 argument");
            return vm->makeNil();
        }

        GLenum cap = args[0].asNumber();
        glDisable(cap);
        return vm->makeNil();
    }

    Value native_glBlendFunc(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glBlendFunc expects 2 arguments");
            return vm->makeNil();
        }

        GLenum sfactor = args[0].asNumber();
        GLenum dfactor = args[1].asNumber();
        glBlendFunc(sfactor, dfactor);
        return vm->makeNil();
    }

    Value native_glDepthFunc(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glDepthFunc expects 1 argument");
            return vm->makeNil();
        }

        GLenum func = args[0].asNumber();
        glDepthFunc(func);
        return vm->makeNil();
    }

    Value native_glDepthMask(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glDepthMask expects 1 argument");
            return vm->makeNil();
        }

        GLboolean flag = args[0].asBool() ? GL_TRUE : GL_FALSE;
        glDepthMask(flag);
        return vm->makeNil();
    }

    Value native_glCullFace(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glCullFace expects 1 argument");
            return vm->makeNil();
        }

        GLenum mode = args[0].asNumber();
        glCullFace(mode);
        return vm->makeNil();
    }

    Value native_glFrontFace(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glFrontFace expects 1 argument");
            return vm->makeNil();
        }

        GLenum mode = args[0].asNumber();
        glFrontFace(mode);
        return vm->makeNil();
    }

    Value native_glPolygonMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glPolygonMode expects 2 arguments");
            return vm->makeNil();
        }

        GLenum face = args[0].asNumber();
        GLenum mode = args[1].asNumber();
        glPolygonMode(face, mode);
        return vm->makeNil();
    }

    Value native_glLineWidth(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glLineWidth expects 1 argument");
            return vm->makeNil();
        }

        GLfloat width = args[0].asNumber();
        glLineWidth(width);
        return vm->makeNil();
    }

    Value native_glPointSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glPointSize expects 1 argument");
            return vm->makeNil();
        }

        GLfloat size = args[0].asNumber();
        glPointSize(size);
        return vm->makeNil();
    }

    Value native_glScissor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("glScissor expects 4 arguments");
            return vm->makeNil();
        }

        GLint x = args[0].asNumber();
        GLint y = args[1].asNumber();
        GLsizei width = args[2].asNumber();
        GLsizei height = args[3].asNumber();
        glScissor(x, y, width, height);
        return vm->makeNil();
    }

    Value native_glGetError(Interpreter *vm, int argc, Value *args)
    {
        GLenum error = glGetError();
        return vm->makeInt(error);
    }

    Value native_glGetString(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glGetString expects 1 argument");
            return vm->makeNil();
        }

        GLenum name = args[0].asNumber();
        const GLubyte *str = glGetString(name);
        if (str)
            return vm->makeString((const char *)str);
        return vm->makeNil();
    }

    Value native_glGetIntegerv(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glGetIntegerv expects 1 argument");
            return vm->makeNil();
        }

        GLenum pname = args[0].asNumber();
        GLint value;
        glGetIntegerv(pname, &value);
        return vm->makeInt(value);
    }

    Value native_glFlush(Interpreter *vm, int argc, Value *args)
    {
        glFlush();
        return vm->makeNil();
    }

    Value native_glFinish(Interpreter *vm, int argc, Value *args)
    {
        glFinish();
        return vm->makeNil();
    }

    // =====================================================
    // IMMEDIATE MODE (OpenGL 1.x - útil para aprender)
    // =====================================================

    Value native_glBegin(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glBegin expects 1 argument");
            return vm->makeNil();
        }

        GLenum mode = args[0].asNumber();
        glBegin(mode);
        return vm->makeNil();
    }

    Value native_glEnd(Interpreter *vm, int argc, Value *args)
    {
        glEnd();
        return vm->makeNil();
    }

    Value native_glVertex2f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glVertex2f expects 2 arguments");
            return vm->makeNil();
        }

        GLfloat x = args[0].asNumber();
        GLfloat y = args[1].asNumber();
        glVertex2f(x, y);
        return vm->makeNil();
    }

    Value native_glVertex3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glVertex3f expects 3 arguments");
            return vm->makeNil();
        }

        GLfloat x = args[0].asNumber();
        GLfloat y = args[1].asNumber();
        GLfloat z = args[2].asNumber();
        glVertex3f(x, y, z);
        return vm->makeNil();
    }

    Value native_glColor3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glColor3f expects 3 arguments");
            return vm->makeNil();
        }

        GLfloat r = args[0].asNumber();
        GLfloat g = args[1].asNumber();
        GLfloat b = args[2].asNumber();
        glColor3f(r, g, b);
        return vm->makeNil();
    }

    Value native_glColor4f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("glColor4f expects 4 arguments");
            return vm->makeNil();
        }

        GLfloat r = args[0].asNumber();
        GLfloat g = args[1].asNumber();
        GLfloat b = args[2].asNumber();
        GLfloat a = args[3].asNumber();
        glColor4f(r, g, b, a);
        return vm->makeNil();
    }

    Value native_glTexCoord2f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glTexCoord2f expects 2 arguments");
            return vm->makeNil();
        }

        GLfloat s = args[0].asNumber();
        GLfloat t = args[1].asNumber();
        glTexCoord2f(s, t);
        return vm->makeNil();
    }

    Value native_glNormal3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glNormal3f expects 3 arguments");
            return vm->makeNil();
        }

        GLfloat nx = args[0].asNumber();
        GLfloat ny = args[1].asNumber();
        GLfloat nz = args[2].asNumber();
        glNormal3f(nx, ny, nz);
        return vm->makeNil();
    }

    // =====================================================
    // MATRIX OPERATIONS (OpenGL 1.x)
    // =====================================================

    Value native_glMatrixMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glMatrixMode expects 1 argument");
            return vm->makeNil();
        }

        GLenum mode = args[0].asNumber();
        glMatrixMode(mode);
        return vm->makeNil();
    }

    Value native_glLoadIdentity(Interpreter *vm, int argc, Value *args)
    {
        glLoadIdentity();
        return vm->makeNil();
    }

    Value native_glPushMatrix(Interpreter *vm, int argc, Value *args)
    {
        glPushMatrix();
        return vm->makeNil();
    }

    Value native_glPopMatrix(Interpreter *vm, int argc, Value *args)
    {
        glPopMatrix();
        return vm->makeNil();
    }

    Value native_glTranslatef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glTranslatef expects 3 arguments");
            return vm->makeNil();
        }

        GLfloat x = args[0].asNumber();
        GLfloat y = args[1].asNumber();
        GLfloat z = args[2].asNumber();
        glTranslatef(x, y, z);
        return vm->makeNil();
    }

    Value native_glRotatef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("glRotatef expects 4 arguments (angle, x, y, z)");
            return vm->makeNil();
        }

        GLfloat angle = args[0].asNumber();
        GLfloat x = args[1].asNumber();
        GLfloat y = args[2].asNumber();
        GLfloat z = args[3].asNumber();
        glRotatef(angle, x, y, z);
        return vm->makeNil();
    }

    Value native_glScalef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glScalef expects 3 arguments");
            return vm->makeNil();
        }

        GLfloat x = args[0].asNumber();
        GLfloat y = args[1].asNumber();
        GLfloat z = args[2].asNumber();
        glScalef(x, y, z);
        return vm->makeNil();
    }

    Value native_glOrtho(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("glOrtho expects 6 arguments");
            return vm->makeNil();
        }

        GLdouble left = args[0].asNumber();
        GLdouble right = args[1].asNumber();
        GLdouble bottom = args[2].asNumber();
        GLdouble top = args[3].asNumber();
        GLdouble zNear = args[4].asNumber();
        GLdouble zFar = args[5].asNumber();
        glOrtho(left, right, bottom, top, zNear, zFar);
        return vm->makeNil();
    }

    void register_opengl(ModuleBuilder &module)
    {
        // =============================================================
        // SDL_GL CONTEXT FUNCTIONS
        // =============================================================
        module.addFunction("SDL_GL_CreateContext", native_SDL_GL_CreateContext, 1)
            .addFunction("SDL_GL_DeleteContext", native_SDL_GL_DeleteContext, 1)
            .addFunction("SDL_GL_MakeCurrent", native_SDL_GL_MakeCurrent, 2)
            .addFunction("SDL_GL_SwapWindow", native_SDL_GL_SwapWindow, 1)
            .addFunction("SDL_GL_SetAttribute", native_SDL_GL_SetAttribute, 2)
            .addFunction("SDL_GL_GetAttribute", native_SDL_GL_GetAttribute, 1)
            .addFunction("SDL_GL_SetSwapInterval", native_SDL_GL_SetSwapInterval, 1)
            .addFunction("SDL_GL_GetSwapInterval", native_SDL_GL_GetSwapInterval, 0)
            .addFunction("SDL_GL_GetCurrentContext", native_SDL_GL_GetCurrentContext, 0)
            .addFunction("SDL_GL_GetCurrentWindow", native_SDL_GL_GetCurrentWindow, 0)

            // =============================================================
            // SDL_GL ATTRIBUTES
            // =============================================================
            .addInt("SDL_GL_RED_SIZE", SDL_GL_RED_SIZE)
            .addInt("SDL_GL_GREEN_SIZE", SDL_GL_GREEN_SIZE)
            .addInt("SDL_GL_BLUE_SIZE", SDL_GL_BLUE_SIZE)
            .addInt("SDL_GL_ALPHA_SIZE", SDL_GL_ALPHA_SIZE)
            .addInt("SDL_GL_BUFFER_SIZE", SDL_GL_BUFFER_SIZE)
            .addInt("SDL_GL_DOUBLEBUFFER", SDL_GL_DOUBLEBUFFER)
            .addInt("SDL_GL_DEPTH_SIZE", SDL_GL_DEPTH_SIZE)
            .addInt("SDL_GL_STENCIL_SIZE", SDL_GL_STENCIL_SIZE)
            .addInt("SDL_GL_CONTEXT_MAJOR_VERSION", SDL_GL_CONTEXT_MAJOR_VERSION)
            .addInt("SDL_GL_CONTEXT_MINOR_VERSION", SDL_GL_CONTEXT_MINOR_VERSION)
            .addInt("SDL_GL_CONTEXT_PROFILE_MASK", SDL_GL_CONTEXT_PROFILE_MASK)
            .addInt("SDL_GL_CONTEXT_PROFILE_CORE", SDL_GL_CONTEXT_PROFILE_CORE)
            .addInt("SDL_GL_CONTEXT_PROFILE_COMPATIBILITY", SDL_GL_CONTEXT_PROFILE_COMPATIBILITY)
            .addInt("SDL_GL_CONTEXT_PROFILE_ES", SDL_GL_CONTEXT_PROFILE_ES)
            .addInt("SDL_GL_MULTISAMPLEBUFFERS", SDL_GL_MULTISAMPLEBUFFERS)
            .addInt("SDL_GL_MULTISAMPLESAMPLES", SDL_GL_MULTISAMPLESAMPLES)

            // =============================================================
            // BASIC OPENGL FUNCTIONS
            // =============================================================
            .addFunction("glClearColor", native_glClearColor, 4)
            .addFunction("glClear", native_glClear, 1)
            .addFunction("glViewport", native_glViewport, 4)
            .addFunction("glEnable", native_glEnable, 1)
            .addFunction("glDisable", native_glDisable, 1)
            .addFunction("glBlendFunc", native_glBlendFunc, 2)
            .addFunction("glDepthFunc", native_glDepthFunc, 1)
            .addFunction("glDepthMask", native_glDepthMask, 1)
            .addFunction("glCullFace", native_glCullFace, 1)
            .addFunction("glFrontFace", native_glFrontFace, 1)
            .addFunction("glPolygonMode", native_glPolygonMode, 2)
            .addFunction("glLineWidth", native_glLineWidth, 1)
            .addFunction("glPointSize", native_glPointSize, 1)
            .addFunction("glScissor", native_glScissor, 4)
            .addFunction("glGetError", native_glGetError, 0)
            .addFunction("glGetString", native_glGetString, 1)
            .addFunction("glGetIntegerv", native_glGetIntegerv, 1)
            .addFunction("glFlush", native_glFlush, 0)
            .addFunction("glFinish", native_glFinish, 0)

            // =============================================================
            // IMMEDIATE MODE (OpenGL 1.x)
            // =============================================================
            .addFunction("glBegin", native_glBegin, 1)
            .addFunction("glEnd", native_glEnd, 0)
            .addFunction("glVertex2f", native_glVertex2f, 2)
            .addFunction("glVertex3f", native_glVertex3f, 3)
            .addFunction("glColor3f", native_glColor3f, 3)
            .addFunction("glColor4f", native_glColor4f, 4)
            .addFunction("glTexCoord2f", native_glTexCoord2f, 2)
            .addFunction("glNormal3f", native_glNormal3f, 3)

            // =============================================================
            // MATRIX OPERATIONS
            // =============================================================
            .addFunction("glMatrixMode", native_glMatrixMode, 1)
            .addFunction("glLoadIdentity", native_glLoadIdentity, 0)
            .addFunction("glPushMatrix", native_glPushMatrix, 0)
            .addFunction("glPopMatrix", native_glPopMatrix, 0)
            .addFunction("glTranslatef", native_glTranslatef, 3)
            .addFunction("glRotatef", native_glRotatef, 4)
            .addFunction("glScalef", native_glScalef, 3)
            .addFunction("glOrtho", native_glOrtho, 6)

            // =============================================================
            // GL CLEAR BUFFER BITS
            // =============================================================
            .addInt("GL_COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT)
            .addInt("GL_DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT)
            .addInt("GL_STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT)

            // =============================================================
            // GL PRIMITIVE TYPES
            // =============================================================
            .addInt("GL_POINTS", GL_POINTS)
            .addInt("GL_LINES", GL_LINES)
            .addInt("GL_LINE_LOOP", GL_LINE_LOOP)
            .addInt("GL_LINE_STRIP", GL_LINE_STRIP)
            .addInt("GL_TRIANGLES", GL_TRIANGLES)
            .addInt("GL_TRIANGLE_STRIP", GL_TRIANGLE_STRIP)
            .addInt("GL_TRIANGLE_FAN", GL_TRIANGLE_FAN)
            .addInt("GL_QUADS", GL_QUADS)
            .addInt("GL_QUAD_STRIP", GL_QUAD_STRIP)
            .addInt("GL_POLYGON", GL_POLYGON)

            // =============================================================
            // GL ENABLE CAPS
            // =============================================================
            .addInt("GL_DEPTH_TEST", GL_DEPTH_TEST)
            .addInt("GL_BLEND", GL_BLEND)
            .addInt("GL_CULL_FACE", GL_CULL_FACE)
            .addInt("GL_SCISSOR_TEST", GL_SCISSOR_TEST)
            .addInt("GL_TEXTURE_2D", GL_TEXTURE_2D)
            .addInt("GL_LINE_SMOOTH", GL_LINE_SMOOTH)
            .addInt("GL_POINT_SMOOTH", GL_POINT_SMOOTH)

            // =============================================================
            // GL BLEND FUNCTIONS
            // =============================================================
            .addInt("GL_ZERO", GL_ZERO)
            .addInt("GL_ONE", GL_ONE)
            .addInt("GL_SRC_COLOR", GL_SRC_COLOR)
            .addInt("GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR)
            .addInt("GL_SRC_ALPHA", GL_SRC_ALPHA)
            .addInt("GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA)
            .addInt("GL_DST_ALPHA", GL_DST_ALPHA)
            .addInt("GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA)
            .addInt("GL_DST_COLOR", GL_DST_COLOR)
            .addInt("GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR)

            // =============================================================
            // GL DEPTH FUNCTIONS
            // =============================================================
            .addInt("GL_NEVER", GL_NEVER)
            .addInt("GL_LESS", GL_LESS)
            .addInt("GL_EQUAL", GL_EQUAL)
            .addInt("GL_LEQUAL", GL_LEQUAL)
            .addInt("GL_GREATER", GL_GREATER)
            .addInt("GL_NOTEQUAL", GL_NOTEQUAL)
            .addInt("GL_GEQUAL", GL_GEQUAL)
            .addInt("GL_ALWAYS", GL_ALWAYS)

            // =============================================================
            // GL CULL FACE
            // =============================================================
            .addInt("GL_FRONT", GL_FRONT)
            .addInt("GL_BACK", GL_BACK)
            .addInt("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK)

            // =============================================================
            // GL FRONT FACE
            // =============================================================
            .addInt("GL_CW", GL_CW)
            .addInt("GL_CCW", GL_CCW)

            // =============================================================
            // GL POLYGON MODE
            // =============================================================
            .addInt("GL_POINT", GL_POINT)
            .addInt("GL_LINE", GL_LINE)
            .addInt("GL_FILL", GL_FILL)

            // =============================================================
            // GL MATRIX MODE
            // =============================================================
            .addInt("GL_MODELVIEW", GL_MODELVIEW)
            .addInt("GL_PROJECTION", GL_PROJECTION)
            .addInt("GL_TEXTURE", GL_TEXTURE)

            // =============================================================
            // GL STRING NAMES
            // =============================================================
            .addInt("GL_VENDOR", GL_VENDOR)
            .addInt("GL_RENDERER", GL_RENDERER)
            .addInt("GL_VERSION", GL_VERSION)
            .addInt("GL_EXTENSIONS", GL_EXTENSIONS)

            // =============================================================
            // GL ERROR CODES
            // =============================================================
            .addInt("GL_NO_ERROR", GL_NO_ERROR)
            .addInt("GL_INVALID_ENUM", GL_INVALID_ENUM)
            .addInt("GL_INVALID_VALUE", GL_INVALID_VALUE)
            .addInt("GL_INVALID_OPERATION", GL_INVALID_OPERATION)
            .addInt("GL_OUT_OF_MEMORY", GL_OUT_OF_MEMORY)

            // =============================================================
            // GL BOOLEAN
            // =============================================================
            .addInt("GL_TRUE", GL_TRUE)
            .addInt("GL_FALSE", GL_FALSE);
    }
}
