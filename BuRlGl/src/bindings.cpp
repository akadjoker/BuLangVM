#include "bindings.hpp"

#define GRAPHICS_API_OPENGL_33
#define RLGL_IMPLEMENTATION
#include "rlgl.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace RLGLBindings
{

    // =============================================================
    // 1. LIFECYCLE E UTILITÁRIOS
    // =============================================================

    // Carrega extensões OpenGL usando glfwGetProcAddress (encontrado via dlsym)
    int native_rlLoadExtensionsGLFW(Interpreter *vm, int argc, Value *args)
    {
#ifndef _WIN32
        typedef void *(*GLFWloadproc)(const char *);
        GLFWloadproc loader = (GLFWloadproc)dlsym(RTLD_DEFAULT, "glfwGetProcAddress");
        if (loader)
        {
            rlLoadExtensions((void *)loader);
            Info("RLGL: OpenGL extensions loaded via GLFW");
            vm->pushBool(true);
            return 1;
        }
        else
        {
            Warning("RLGL: glfwGetProcAddress not found");
            vm->pushBool(false);
            return 1;
        }
#else
        // Windows: usar GetProcAddress do kernel32
        Warning("RLGL: Windows loader not implemented yet");
        vm->pushBool(false);
        return 1;
#endif
    }

    // Carrega extensões OpenGL usando SDL_GL_GetProcAddress (encontrado via dlsym)
    int native_rlLoadExtensionsSDL(Interpreter *vm, int argc, Value *args)
    {
#ifndef _WIN32
        typedef void *(*SDLloadproc)(const char *);
        SDLloadproc loader = (SDLloadproc)dlsym(RTLD_DEFAULT, "SDL_GL_GetProcAddress");
        if (loader)
        {
            rlLoadExtensions((void *)loader);
            Info("RLGL: OpenGL extensions loaded via SDL");
            vm->pushBool(true);
            return 1;
        }
        else
        {
            Warning("RLGL: SDL_GL_GetProcAddress not found");
            vm->pushBool(false);
            return 1;
        }
#else
        Warning("RLGL: Windows loader not implemented yet");
        vm->pushBool(false);
        return 1;
#endif
    }

    int native_rlglInit(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlglInit(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlglClose(Interpreter *vm, int argc, Value *args)
    {
        rlglClose();
        return 0;
    }

    int native_rlGetVersion(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(rlGetVersion());
        return 1;
    }

    int native_rlCheckErrors(Interpreter *vm, int argc, Value *args)
    {
        rlCheckErrors();
        return 0;
    }

    // =============================================================
    // 2. MATRIZES
    // =============================================================

    int native_rlMatrixMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        rlMatrixMode(args[0].asNumber());
        return 0;
    }

    int native_rlPushMatrix(Interpreter *vm, int argc, Value *args)
    {
        rlPushMatrix();
        return 0;
    }

    int native_rlPopMatrix(Interpreter *vm, int argc, Value *args)
    {
        rlPopMatrix();
        return 0;
    }

    int native_rlLoadIdentity(Interpreter *vm, int argc, Value *args)
    {
        rlLoadIdentity();
        return 0;
    }

    int native_rlTranslatef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlTranslatef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlRotatef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlRotatef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    int native_rlScalef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlScalef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlOrtho(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
            return 0;
        rlOrtho(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber());
        return 0;
    }

    int native_rlFrustum(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
            return 0;
        rlFrustum(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber());
        return 0;
    }

    int native_rlViewport(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlViewport(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    // =============================================================
    // 3. DESENHO (IMMEDIATE MODE)
    // =============================================================

    int native_rlBegin(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        rlBegin(args[0].asNumber());
        return 0;
    }

    int native_rlEnd(Interpreter *vm, int argc, Value *args)
    {
        rlEnd();
        return 0;
    }

    int native_rlVertex2f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlVertex2f(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlVertex3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlVertex3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlTexCoord2f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlTexCoord2f(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlNormal3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlNormal3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlColor4f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlColor4f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    int native_rlColor3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlColor3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    // =============================================================
    // 4. ESTADOS DE RENDERIZAÇÃO
    // =============================================================

    int native_rlEnableDepthTest(Interpreter *vm, int argc, Value *args)
    {
        rlEnableDepthTest();
        return 0;
    }
    int native_rlDisableDepthTest(Interpreter *vm, int argc, Value *args)
    {
        rlDisableDepthTest();
        return 0;
    }
    int native_rlEnableDepthMask(Interpreter *vm, int argc, Value *args)
    {
        rlEnableDepthMask();
        return 0;
    }
    int native_rlDisableDepthMask(Interpreter *vm, int argc, Value *args)
    {
        rlDisableDepthMask();
        return 0;
    }

    int native_rlEnableBackfaceCulling(Interpreter *vm, int argc, Value *args)
    {
        rlEnableBackfaceCulling();
        return 0;
    }
    int native_rlDisableBackfaceCulling(Interpreter *vm, int argc, Value *args)
    {
        rlDisableBackfaceCulling();
        return 0;
    }
    int native_rlSetCullFace(Interpreter *vm, int argc, Value *args)
    {
        rlSetCullFace(args[0].asNumber());
        return 0;
    }

    int native_rlEnableScissorTest(Interpreter *vm, int argc, Value *args)
    {
        rlEnableScissorTest();
        return 0;
    }
    int native_rlDisableScissorTest(Interpreter *vm, int argc, Value *args)
    {
        rlDisableScissorTest();
        return 0;
    }
    int native_rlScissor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlScissor(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    int native_rlEnableWireMode(Interpreter *vm, int argc, Value *args)
    {
        rlEnableWireMode();
        return 0;
    }
    int native_rlEnablePointMode(Interpreter *vm, int argc, Value *args)
    {
        rlEnablePointMode();
        return 0;
    }
    int native_rlDisableWireMode(Interpreter *vm, int argc, Value *args)
    {
        rlDisableWireMode();
        return 0;
    }

    int native_rlSetLineWidth(Interpreter *vm, int argc, Value *args)
    {
        rlSetLineWidth(args[0].asNumber());
        return 0;
    }

    int native_rlEnableColorBlend(Interpreter *vm, int argc, Value *args)
    {
        rlEnableColorBlend();
        return 0;
    }
    int native_rlDisableColorBlend(Interpreter *vm, int argc, Value *args)
    {
        rlDisableColorBlend();
        return 0;
    }
    int native_rlSetBlendMode(Interpreter *vm, int argc, Value *args)
    {
        rlSetBlendMode(args[0].asNumber());
        return 0;
    }

    int native_rlClearColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlClearColor((uint8_t)args[0].asNumber(), (uint8_t)args[1].asNumber(), (uint8_t)args[2].asNumber(), (uint8_t)args[3].asNumber());
        return 0;
    }
    int native_rlClearScreenBuffers(Interpreter *vm, int argc, Value *args)
    {
        rlClearScreenBuffers();
        return 0;
    }

    // =============================================================
    // 5. VERTEX ARRAYS & BUFFERS
    // =============================================================

    int native_rlLoadVertexArray(Interpreter *vm, int argc, Value *args)
    {
         vm->pushInt(rlLoadVertexArray());
        return 1;
    }

    int native_rlLoadVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        // buffer (void*), size, dynamic
        void *ptr = nullptr;
        int size = args[1].asNumber();
        bool dynamic = args[2].asBool();
         vm->pushInt(rlLoadVertexBuffer(ptr, size, dynamic));
        return 1;
    }

    int native_rlEnableVertexArray(Interpreter *vm, int argc, Value *args)
    {
        rlEnableVertexArray(args[0].asNumber());
        return 0;
    }
    int native_rlDisableVertexArray(Interpreter *vm, int argc, Value *args)
    {
        rlDisableVertexArray();
        return 0;
    }

    int native_rlEnableVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        rlEnableVertexBuffer(args[0].asNumber());
        return 0;
    }
    int native_rlDisableVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        rlDisableVertexBuffer();
        return 0;
    }

    int native_rlUnloadVertexArray(Interpreter *vm, int argc, Value *args)
    {
        rlUnloadVertexArray(args[0].asNumber());
        return 0;
    }
    int native_rlUnloadVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        rlUnloadVertexBuffer(args[0].asNumber());
        return 0;
    }

    // =============================================================
    // 6. FRAMEBUFFERS & TEXTURAS
    // =============================================================

    int native_rlLoadFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        vm->pushInt(rlLoadFramebuffer(args[0].asNumber(), args[1].asNumber()));
        return 1;
    }

    int native_rlFramebufferAttach(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
            return 0;
        rlFramebufferAttach(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber());
        return 0;
    }

    int native_rlFramebufferComplete(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushBool(rlFramebufferComplete(args[0].asNumber()));
        return 1;
    }

    int native_rlUnloadFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        rlUnloadFramebuffer(args[0].asNumber());
        return 0;
    }

    int native_rlEnableFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        rlEnableFramebuffer(args[0].asNumber());
        return 0;
    }
    int native_rlDisableFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        rlDisableFramebuffer();
        return 0;
    }

    int native_rlActiveTextureSlot(Interpreter *vm, int argc, Value *args)
    {
        rlActiveTextureSlot(args[0].asNumber());
        return 0;
    }
    int native_rlEnableTexture(Interpreter *vm, int argc, Value *args)
    {
        rlEnableTexture(args[0].asNumber());
        return 0;
    }
    int native_rlDisableTexture(Interpreter *vm, int argc, Value *args)
    {
        rlDisableTexture();
        return 0;
    }

    int native_rlTextureParameters(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlTextureParameters(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlGenTextureMipmaps(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        int mipmaps = 1;
        rlGenTextureMipmaps(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), &mipmaps);
        vm->pushInt(mipmaps);
        return 1;
    }

    // =============================================================
    // 7. SHADERS (Vertex, Fragment, Compute)
    // =============================================================

    int native_rlLoadShaderCode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        const char *vs = args[0].isString() ? args[0].asStringChars() : nullptr;
        const char *fs = args[1].isString() ? args[1].asStringChars() : nullptr;
       vm->pushInt(rlLoadShaderCode(vs, fs));
        return 1;
    }

    int native_rlCompileShader(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        vm->pushInt(rlCompileShader(args[0].asStringChars(), args[1].asNumber()));
        return 1;
    }

    int native_rlLoadShaderProgram(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        vm->pushInt(rlLoadShaderProgram(args[0].asNumber(), args[1].asNumber()));
        return 1;
    }

    int native_rlUnloadShaderProgram(Interpreter *vm, int argc, Value *args)
    {
        rlUnloadShaderProgram(args[0].asNumber());
        return 0;
    }

    int native_rlEnableShader(Interpreter *vm, int argc, Value *args)
    {
        rlEnableShader(args[0].asNumber());
        return 0;
    }
    int native_rlDisableShader(Interpreter *vm, int argc, Value *args)
    {
        rlDisableShader();
        return 0;
    }

    int native_rlGetLocationUniform(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)            
        {
            return 0;
        }
        vm->pushInt(rlGetLocationUniform(args[0].asNumber(), args[1].asStringChars()));
        return 1;
    }

    int native_rlSetUniform(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 3)
            return 0;
        int loc = args[0].asNumber();
        int type = args[2].asNumber();

        if (type == RL_SHADER_UNIFORM_FLOAT)
        {
            float val = args[1].asNumber();
            rlSetUniform(loc, &val, type, 1);
        }
        else if (type == RL_SHADER_UNIFORM_INT)
        {
            int val = args[1].asNumber();
            rlSetUniform(loc, &val, type, 1);
        }
        else if (type == RL_SHADER_UNIFORM_VEC3)
        {
            float vec[3];
            if (vm->getVec3(args[1], vec))
            {
                rlSetUniform(loc, vec, type, 1);
            }
        }
        else if (type == RL_SHADER_UNIFORM_VEC4)
        {
            float vec[4];
            if (vm->getVec4(args[1], vec))
            {
                rlSetUniform(loc, vec, type, 1);
            }
        }
        return 0;
    }

    int native_rlSetUniformSampler(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlSetUniformSampler(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlLoadComputeShaderProgram(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushInt(rlLoadComputeShaderProgram(args[0].asNumber()));
        return 1;
    }

    int native_rlComputeShaderDispatch(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlComputeShaderDispatch(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlDrawRenderBatchActive(Interpreter *vm, int argc, Value *args)
    {
        rlDrawRenderBatchActive();
        return 0;
    }

    // =============================================================
    // REGISTO FINAL
    // =============================================================

    void register_rlgl(ModuleBuilder &mod)
    {
        mod
            // --- LIFECYCLE ---
            .addFunction("rlLoadExtensionsGLFW", native_rlLoadExtensionsGLFW, 0)
            .addFunction("rlLoadExtensionsSDL", native_rlLoadExtensionsSDL, 0)
            .addFunction("rlglInit", native_rlglInit, 2)
            .addFunction("rlglClose", native_rlglClose, 0)
            .addFunction("rlGetVersion", native_rlGetVersion, 0)
            .addFunction("rlCheckErrors", native_rlCheckErrors, 0)

            // --- MATRIX ---
            .addFunction("rlMatrixMode", native_rlMatrixMode, 1)
            .addFunction("rlPushMatrix", native_rlPushMatrix, 0)
            .addFunction("rlPopMatrix", native_rlPopMatrix, 0)
            .addFunction("rlLoadIdentity", native_rlLoadIdentity, 0)
            .addFunction("rlTranslatef", native_rlTranslatef, 3)
            .addFunction("rlRotatef", native_rlRotatef, 4)
            .addFunction("rlScalef", native_rlScalef, 3)
            .addFunction("rlOrtho", native_rlOrtho, 6)
            .addFunction("rlFrustum", native_rlFrustum, 6)
            .addFunction("rlViewport", native_rlViewport, 4)

            // --- IMMEDIATE MODE ---
            .addFunction("rlBegin", native_rlBegin, 1)
            .addFunction("rlEnd", native_rlEnd, 0)
            .addFunction("rlVertex2f", native_rlVertex2f, 2)
            .addFunction("rlVertex3f", native_rlVertex3f, 3)
            .addFunction("rlTexCoord2f", native_rlTexCoord2f, 2)
            .addFunction("rlNormal3f", native_rlNormal3f, 3)
            .addFunction("rlColor4f", native_rlColor4f, 4)
            .addFunction("rlColor3f", native_rlColor3f, 3)

            // --- STATES ---
            .addFunction("rlEnableDepthTest", native_rlEnableDepthTest, 0)
            .addFunction("rlDisableDepthTest", native_rlDisableDepthTest, 0)
            .addFunction("rlEnableDepthMask", native_rlEnableDepthMask, 0)
            .addFunction("rlDisableDepthMask", native_rlDisableDepthMask, 0)
            .addFunction("rlEnableBackfaceCulling", native_rlEnableBackfaceCulling, 0)
            .addFunction("rlDisableBackfaceCulling", native_rlDisableBackfaceCulling, 0)
            .addFunction("rlSetCullFace", native_rlSetCullFace, 1)
            .addFunction("rlEnableScissorTest", native_rlEnableScissorTest, 0)
            .addFunction("rlDisableScissorTest", native_rlDisableScissorTest, 0)
            .addFunction("rlScissor", native_rlScissor, 4)
            .addFunction("rlEnableWireMode", native_rlEnableWireMode, 0)
            .addFunction("rlEnablePointMode", native_rlEnablePointMode, 0)
            .addFunction("rlDisableWireMode", native_rlDisableWireMode, 0)
            .addFunction("rlSetLineWidth", native_rlSetLineWidth, 1)
            .addFunction("rlEnableColorBlend", native_rlEnableColorBlend, 0)
            .addFunction("rlDisableColorBlend", native_rlDisableColorBlend, 0)
            .addFunction("rlSetBlendMode", native_rlSetBlendMode, 1)
            .addFunction("rlClearColor", native_rlClearColor, 4)
            .addFunction("rlClearScreenBuffers", native_rlClearScreenBuffers, 0)
            .addFunction("rlDrawRenderBatchActive", native_rlDrawRenderBatchActive, 0)

            // --- VERTEX ARRAYS & BUFFERS ---
            .addFunction("rlLoadVertexArray", native_rlLoadVertexArray, 0)
            .addFunction("rlLoadVertexBuffer", native_rlLoadVertexBuffer, 3)
            .addFunction("rlEnableVertexArray", native_rlEnableVertexArray, 1)
            .addFunction("rlDisableVertexArray", native_rlDisableVertexArray, 0)
            .addFunction("rlEnableVertexBuffer", native_rlEnableVertexBuffer, 1)
            .addFunction("rlDisableVertexBuffer", native_rlDisableVertexBuffer, 0)
            .addFunction("rlUnloadVertexArray", native_rlUnloadVertexArray, 1)
            .addFunction("rlUnloadVertexBuffer", native_rlUnloadVertexBuffer, 1)

            // --- FRAMEBUFFERS & TEXTURES ---
            .addFunction("rlLoadFramebuffer", native_rlLoadFramebuffer, 2)
            .addFunction("rlFramebufferAttach", native_rlFramebufferAttach, 5)
            .addFunction("rlFramebufferComplete", native_rlFramebufferComplete, 1)
            .addFunction("rlUnloadFramebuffer", native_rlUnloadFramebuffer, 1)
            .addFunction("rlEnableFramebuffer", native_rlEnableFramebuffer, 1)
            .addFunction("rlDisableFramebuffer", native_rlDisableFramebuffer, 0)
            .addFunction("rlActiveTextureSlot", native_rlActiveTextureSlot, 1)
            .addFunction("rlEnableTexture", native_rlEnableTexture, 1)
            .addFunction("rlDisableTexture", native_rlDisableTexture, 0)
            .addFunction("rlTextureParameters", native_rlTextureParameters, 3)
            .addFunction("rlGenTextureMipmaps", native_rlGenTextureMipmaps, 4)

            // --- SHADERS ---
            .addFunction("rlLoadShaderCode", native_rlLoadShaderCode, 2)
            .addFunction("rlCompileShader", native_rlCompileShader, 2)
            .addFunction("rlLoadShaderProgram", native_rlLoadShaderProgram, 2)
            .addFunction("rlUnloadShaderProgram", native_rlUnloadShaderProgram, 1)
            .addFunction("rlEnableShader", native_rlEnableShader, 1)
            .addFunction("rlDisableShader", native_rlDisableShader, 0)
            .addFunction("rlGetLocationUniform", native_rlGetLocationUniform, 2)
            .addFunction("rlSetUniform", native_rlSetUniform, 4)
            .addFunction("rlSetUniformSampler", native_rlSetUniformSampler, 2)
            .addFunction("rlLoadComputeShaderProgram", native_rlLoadComputeShaderProgram, 1)
            .addFunction("rlComputeShaderDispatch", native_rlComputeShaderDispatch, 3)

            // ---------------- CONSTANTES COMPLETAS ----------------

            // Primitives
            .addByte("RL_LINES", RL_LINES)
            .addByte("RL_TRIANGLES", RL_TRIANGLES)
            .addByte("RL_QUADS", RL_QUADS)

            // Matrix Modes
            .addInt("RL_MODELVIEW", RL_MODELVIEW)
            .addInt("RL_PROJECTION", RL_PROJECTION)
            .addInt("RL_TEXTURE", RL_TEXTURE)

            // Culling
            .addByte("RL_CULL_FACE_FRONT", RL_CULL_FACE_FRONT)
            .addByte("RL_CULL_FACE_BACK", RL_CULL_FACE_BACK)

            // Texture Parameters
            .addInt("RL_TEXTURE_WRAP_S", RL_TEXTURE_WRAP_S)
            .addInt("RL_TEXTURE_WRAP_T", RL_TEXTURE_WRAP_T)
            .addInt("RL_TEXTURE_MAG_FILTER", RL_TEXTURE_MAG_FILTER)
            .addInt("RL_TEXTURE_MIN_FILTER", RL_TEXTURE_MIN_FILTER)
            .addInt("RL_TEXTURE_FILTER_NEAREST", RL_TEXTURE_FILTER_NEAREST)
            .addInt("RL_TEXTURE_FILTER_LINEAR", RL_TEXTURE_FILTER_LINEAR)
            .addInt("RL_TEXTURE_FILTER_MIP_NEAREST", RL_TEXTURE_FILTER_MIP_NEAREST)
            .addInt("RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR", RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR)
            .addInt("RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST", RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST)
            .addInt("RL_TEXTURE_FILTER_MIP_LINEAR", RL_TEXTURE_FILTER_MIP_LINEAR)
            .addInt("RL_TEXTURE_FILTER_ANISOTROPIC", RL_TEXTURE_FILTER_ANISOTROPIC)

            .addInt("RL_TEXTURE_WRAP_REPEAT", RL_TEXTURE_WRAP_REPEAT)
            .addInt("RL_TEXTURE_WRAP_CLAMP", RL_TEXTURE_WRAP_CLAMP)
            .addInt("RL_TEXTURE_WRAP_MIRROR_REPEAT", RL_TEXTURE_WRAP_MIRROR_REPEAT)
            .addInt("RL_TEXTURE_WRAP_MIRROR_CLAMP", RL_TEXTURE_WRAP_MIRROR_CLAMP)

            // Blend Modes
            .addByte("RL_BLEND_ALPHA", RL_BLEND_ALPHA)
            .addByte("RL_BLEND_ADDITIVE", RL_BLEND_ADDITIVE)
            .addByte("RL_BLEND_MULTIPLIED", RL_BLEND_MULTIPLIED)
            .addByte("RL_BLEND_ADD_COLORS", RL_BLEND_ADD_COLORS)
            .addByte("RL_BLEND_SUBTRACT_COLORS", RL_BLEND_SUBTRACT_COLORS)
            .addByte("RL_BLEND_ALPHA_PREMULTIPLY", RL_BLEND_ALPHA_PREMULTIPLY)
            .addByte("RL_BLEND_CUSTOM", RL_BLEND_CUSTOM)

            // Attachments (COMPLETO)
            .addByte("RL_ATTACHMENT_COLOR_CHANNEL0", RL_ATTACHMENT_COLOR_CHANNEL0)
            .addByte("RL_ATTACHMENT_COLOR_CHANNEL1", RL_ATTACHMENT_COLOR_CHANNEL1)
            .addByte("RL_ATTACHMENT_COLOR_CHANNEL2", RL_ATTACHMENT_COLOR_CHANNEL2)
            .addByte("RL_ATTACHMENT_DEPTH", RL_ATTACHMENT_DEPTH)
            .addByte("RL_ATTACHMENT_STENCIL", RL_ATTACHMENT_STENCIL)
            .addByte("RL_ATTACHMENT_TEXTURE2D", RL_ATTACHMENT_TEXTURE2D)
            .addByte("RL_ATTACHMENT_RENDERBUFFER", RL_ATTACHMENT_RENDERBUFFER)
            .addByte("RL_ATTACHMENT_CUBEMAP_POSITIVE_X", RL_ATTACHMENT_CUBEMAP_POSITIVE_X)
            .addByte("RL_ATTACHMENT_CUBEMAP_NEGATIVE_X", RL_ATTACHMENT_CUBEMAP_NEGATIVE_X)
            .addByte("RL_ATTACHMENT_CUBEMAP_POSITIVE_Y", RL_ATTACHMENT_CUBEMAP_POSITIVE_Y)
            .addByte("RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y", RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y)
            .addByte("RL_ATTACHMENT_CUBEMAP_POSITIVE_Z", RL_ATTACHMENT_CUBEMAP_POSITIVE_Z)
            .addByte("RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z", RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z)

            // Shader Types
            .addInt("RL_FRAGMENT_SHADER", RL_FRAGMENT_SHADER)
            .addInt("RL_VERTEX_SHADER", RL_VERTEX_SHADER)
            .addInt("RL_COMPUTE_SHADER", RL_COMPUTE_SHADER)

            // Uniform Types
            .addByte("RL_SHADER_UNIFORM_FLOAT", RL_SHADER_UNIFORM_FLOAT)
            .addByte("RL_SHADER_UNIFORM_VEC2", RL_SHADER_UNIFORM_VEC2)
            .addByte("RL_SHADER_UNIFORM_VEC3", RL_SHADER_UNIFORM_VEC3)
            .addByte("RL_SHADER_UNIFORM_VEC4", RL_SHADER_UNIFORM_VEC4)
            .addByte("RL_SHADER_UNIFORM_INT", RL_SHADER_UNIFORM_INT)
            .addByte("RL_SHADER_UNIFORM_IVEC2", RL_SHADER_UNIFORM_IVEC2)
            .addByte("RL_SHADER_UNIFORM_IVEC3", RL_SHADER_UNIFORM_IVEC3)
            .addByte("RL_SHADER_UNIFORM_IVEC4", RL_SHADER_UNIFORM_IVEC4)
            .addByte("RL_SHADER_UNIFORM_SAMPLER2D", RL_SHADER_UNIFORM_SAMPLER2D);
    }

    void registerAll(Interpreter &vm)
    {
        ModuleBuilder module = vm.addModule("RLGL");
        register_rlgl(module);
    }
}
