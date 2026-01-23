#include "bindings.hpp"
//#define RLGL_IMPLEMENTATION
#include "rlgl.h"

namespace RaylibBindings
{
 
    // =============================================================
    // 1. LIFECYCLE E UTILITÁRIOS
    // =============================================================
 
    Value native_rlGetVersion(Interpreter *vm, int argc, Value *args) {
        return vm->makeInt(rlGetVersion());
    }

    Value native_rlCheckErrors(Interpreter *vm, int argc, Value *args) {
        rlCheckErrors();
        return vm->makeNil();
    }

    // =============================================================
    // 2. MATRIZES
    // =============================================================

    Value native_rlMatrixMode(Interpreter *vm, int argc, Value *args) {
        if (argc != 1) return vm->makeNil();
        rlMatrixMode(args[0].asNumber());
        return vm->makeNil();
    }

    Value native_rlPushMatrix(Interpreter *vm, int argc, Value *args) {
        rlPushMatrix();
        return vm->makeNil();
    }

    Value native_rlPopMatrix(Interpreter *vm, int argc, Value *args) {
        rlPopMatrix();
        return vm->makeNil();
    }

    Value native_rlLoadIdentity(Interpreter *vm, int argc, Value *args) {
        rlLoadIdentity();
        return vm->makeNil();
    }

    Value native_rlTranslatef(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlTranslatef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    Value native_rlRotatef(Interpreter *vm, int argc, Value *args) {
        if (argc != 4) return vm->makeNil();
        rlRotatef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return vm->makeNil();
    }

    Value native_rlScalef(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlScalef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    Value native_rlOrtho(Interpreter *vm, int argc, Value *args) {
        if (argc != 6) return vm->makeNil();
        rlOrtho(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber());
        return vm->makeNil();
    }

    Value native_rlFrustum(Interpreter *vm, int argc, Value *args) {
        if (argc != 6) return vm->makeNil();
        rlFrustum(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber());
        return vm->makeNil();
    }

    Value native_rlViewport(Interpreter *vm, int argc, Value *args) {
        if (argc != 4) return vm->makeNil();
        rlViewport(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return vm->makeNil();
    }

    // =============================================================
    // 3. DESENHO (IMMEDIATE MODE)
    // =============================================================

    Value native_rlBegin(Interpreter *vm, int argc, Value *args) {
        if (argc != 1) return vm->makeNil();
        rlBegin(args[0].asNumber());
        return vm->makeNil();
    }

    Value native_rlEnd(Interpreter *vm, int argc, Value *args) {
        rlEnd();
        return vm->makeNil();
    }

    Value native_rlVertex2f(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        rlVertex2f(args[0].asNumber(), args[1].asNumber());
        return vm->makeNil();
    }

    Value native_rlVertex3f(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlVertex3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    Value native_rlTexCoord2f(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        rlTexCoord2f(args[0].asNumber(), args[1].asNumber());
        return vm->makeNil();
    }

    Value native_rlNormal3f(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlNormal3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    Value native_rlColor4f(Interpreter *vm, int argc, Value *args) {
        if (argc != 4) return vm->makeNil();
        rlColor4f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return vm->makeNil();
    }

    Value native_rlColor3f(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlColor3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    // =============================================================
    // 4. ESTADOS DE RENDERIZAÇÃO
    // =============================================================

    Value native_rlEnableDepthTest(Interpreter *vm, int argc, Value *args) { rlEnableDepthTest(); return vm->makeNil(); }
    Value native_rlDisableDepthTest(Interpreter *vm, int argc, Value *args) { rlDisableDepthTest(); return vm->makeNil(); }
    Value native_rlEnableDepthMask(Interpreter *vm, int argc, Value *args) { rlEnableDepthMask(); return vm->makeNil(); }
    Value native_rlDisableDepthMask(Interpreter *vm, int argc, Value *args) { rlDisableDepthMask(); return vm->makeNil(); }

    Value native_rlEnableBackfaceCulling(Interpreter *vm, int argc, Value *args) { rlEnableBackfaceCulling(); return vm->makeNil(); }
    Value native_rlDisableBackfaceCulling(Interpreter *vm, int argc, Value *args) { rlDisableBackfaceCulling(); return vm->makeNil(); }
    Value native_rlSetCullFace(Interpreter *vm, int argc, Value *args) { rlSetCullFace(args[0].asNumber()); return vm->makeNil(); }

    Value native_rlEnableScissorTest(Interpreter *vm, int argc, Value *args) { rlEnableScissorTest(); return vm->makeNil(); }
    Value native_rlDisableScissorTest(Interpreter *vm, int argc, Value *args) { rlDisableScissorTest(); return vm->makeNil(); }
    Value native_rlScissor(Interpreter *vm, int argc, Value *args) {
        if (argc != 4) return vm->makeNil();
        rlScissor(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return vm->makeNil();
    }

    Value native_rlEnableWireMode(Interpreter *vm, int argc, Value *args) { rlEnableWireMode(); return vm->makeNil(); }
    Value native_rlEnablePointMode(Interpreter *vm, int argc, Value *args) { rlEnablePointMode(); return vm->makeNil(); }
    Value native_rlDisableWireMode(Interpreter *vm, int argc, Value *args) { rlDisableWireMode(); return vm->makeNil(); }

    Value native_rlSetLineWidth(Interpreter *vm, int argc, Value *args) { rlSetLineWidth(args[0].asNumber()); return vm->makeNil(); }

    Value native_rlEnableColorBlend(Interpreter *vm, int argc, Value *args) { rlEnableColorBlend(); return vm->makeNil(); }
    Value native_rlDisableColorBlend(Interpreter *vm, int argc, Value *args) { rlDisableColorBlend(); return vm->makeNil(); }
    Value native_rlSetBlendMode(Interpreter *vm, int argc, Value *args) { rlSetBlendMode(args[0].asNumber()); return vm->makeNil(); }

    Value native_rlClearColor(Interpreter *vm, int argc, Value *args) {
        if (argc != 4) return vm->makeNil();
        rlClearColor((uint8_t)args[0].asNumber(), (uint8_t)args[1].asNumber(), (uint8_t)args[2].asNumber(), (uint8_t)args[3].asNumber());
        return vm->makeNil();
    }
    Value native_rlClearScreenBuffers(Interpreter *vm, int argc, Value *args) { rlClearScreenBuffers(); return vm->makeNil(); }

    // =============================================================
    // 5. VERTEX ARRAYS & BUFFERS
    // =============================================================

    Value native_rlLoadVertexArray(Interpreter *vm, int argc, Value *args) {
        return vm->makeInt(rlLoadVertexArray());
    }

    Value native_rlLoadVertexBuffer(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        // buffer (void*), size, dynamic
        void* ptr = nullptr; 
        int size = args[1].asNumber();
        bool dynamic = args[2].asBool();
        return vm->makeInt(rlLoadVertexBuffer(ptr, size, dynamic));
    }

    Value native_rlEnableVertexArray(Interpreter *vm, int argc, Value *args) { rlEnableVertexArray(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlDisableVertexArray(Interpreter *vm, int argc, Value *args) { rlDisableVertexArray(); return vm->makeNil(); }

    Value native_rlEnableVertexBuffer(Interpreter *vm, int argc, Value *args) { rlEnableVertexBuffer(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlDisableVertexBuffer(Interpreter *vm, int argc, Value *args) { rlDisableVertexBuffer(); return vm->makeNil(); }

    Value native_rlUnloadVertexArray(Interpreter *vm, int argc, Value *args) { rlUnloadVertexArray(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlUnloadVertexBuffer(Interpreter *vm, int argc, Value *args) { rlUnloadVertexBuffer(args[0].asNumber()); return vm->makeNil(); }

    // =============================================================
    // 6. FRAMEBUFFERS & TEXTURAS
    // =============================================================

    Value native_rlLoadFramebuffer(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        return vm->makeInt(rlLoadFramebuffer(args[0].asNumber(), args[1].asNumber()));
    }

    Value native_rlFramebufferAttach(Interpreter *vm, int argc, Value *args) {
        if (argc != 5) return vm->makeNil();
        rlFramebufferAttach(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber());
        return vm->makeNil();
    }

    Value native_rlFramebufferComplete(Interpreter *vm, int argc, Value *args) {
        if (argc != 1) return vm->makeBool(false);
        return vm->makeBool(rlFramebufferComplete(args[0].asNumber()));
    }

    Value native_rlUnloadFramebuffer(Interpreter *vm, int argc, Value *args) {
        if (argc != 1) return vm->makeNil();
        rlUnloadFramebuffer(args[0].asNumber());
        return vm->makeNil();
    }

    Value native_rlEnableFramebuffer(Interpreter *vm, int argc, Value *args) { rlEnableFramebuffer(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlDisableFramebuffer(Interpreter *vm, int argc, Value *args) { rlDisableFramebuffer(); return vm->makeNil(); }

    Value native_rlActiveTextureSlot(Interpreter *vm, int argc, Value *args) { rlActiveTextureSlot(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlEnableTexture(Interpreter *vm, int argc, Value *args) { rlEnableTexture(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlDisableTexture(Interpreter *vm, int argc, Value *args) { rlDisableTexture(); return vm->makeNil(); }

    Value native_rlTextureParameters(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlTextureParameters(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    Value native_rlGenTextureMipmaps(Interpreter *vm, int argc, Value *args) {
        if (argc != 4) return vm->makeNil();
        int mipmaps = 1;
        rlGenTextureMipmaps(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), &mipmaps);
        return vm->makeInt(mipmaps);
    }

    // =============================================================
    // 7. SHADERS (Vertex, Fragment, Compute)
    // =============================================================

    Value native_rlLoadShaderCode(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        const char* vs = args[0].isString() ? args[0].asStringChars() : nullptr;
        const char* fs = args[1].isString() ? args[1].asStringChars() : nullptr;
        return vm->makeInt(rlLoadShaderCode(vs, fs));
    }

    Value native_rlCompileShader(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        return vm->makeInt(rlCompileShader(args[0].asStringChars(), args[1].asNumber()));
    }

    Value native_rlLoadShaderProgram(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        return vm->makeInt(rlLoadShaderProgram(args[0].asNumber(), args[1].asNumber()));
    }

    Value native_rlUnloadShaderProgram(Interpreter *vm, int argc, Value *args) { rlUnloadShaderProgram(args[0].asNumber()); return vm->makeNil(); }

    Value native_rlEnableShader(Interpreter *vm, int argc, Value *args) { rlEnableShader(args[0].asNumber()); return vm->makeNil(); }
    Value native_rlDisableShader(Interpreter *vm, int argc, Value *args) { rlDisableShader(); return vm->makeNil(); }

    Value native_rlGetLocationUniform(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeInt(-1);
        return vm->makeInt(rlGetLocationUniform(args[0].asNumber(), args[1].asStringChars()));
    }

    Value native_rlSetUniform(Interpreter *vm, int argc, Value *args) {
        // locIndex, value, type, count
        if (argc < 3) return vm->makeNil();
        int loc = args[0].asNumber();
        int type = args[2].asNumber();
        
        if (type == RL_SHADER_UNIFORM_FLOAT) {
            float val = args[1].asNumber();
            rlSetUniform(loc, &val, type, 1);
        } else if (type == RL_SHADER_UNIFORM_INT) {
            int val = args[1].asNumber();
            rlSetUniform(loc, &val, type, 1);
        } else if (type == RL_SHADER_UNIFORM_VEC3) {
            // Se o utilizador passar um array de 3 elementos na VM, teríamos de extrair.
            // Para simplificar, assumimos que neste binding básico não suportamos passar structs complexas
            // sem Helpers específicos. Use rlSetUniformVec3 abaixo para isso.
        }
        return vm->makeNil();
    }

    Value native_rlSetUniformSampler(Interpreter *vm, int argc, Value *args) {
        if (argc != 2) return vm->makeNil();
        rlSetUniformSampler(args[0].asNumber(), args[1].asNumber());
        return vm->makeNil();
    }

    Value native_rlLoadComputeShaderProgram(Interpreter *vm, int argc, Value *args) {
        if (argc != 1) return vm->makeNil();
        return vm->makeInt(rlLoadComputeShaderProgram(args[0].asNumber()));
    }

    Value native_rlComputeShaderDispatch(Interpreter *vm, int argc, Value *args) {
        if (argc != 3) return vm->makeNil();
        rlComputeShaderDispatch(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    // =============================================================
    // REGISTO FINAL
    // =============================================================

    void register_rlgl(ModuleBuilder &mod)
    {
        mod
            // --- LIFECYCLE ---
             
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
            .addInt("RL_LINES", RL_LINES)
            .addInt("RL_TRIANGLES", RL_TRIANGLES)
            .addInt("RL_QUADS", RL_QUADS)

            // Matrix Modes
            .addInt("RL_MODELVIEW", RL_MODELVIEW)
            .addInt("RL_PROJECTION", RL_PROJECTION)
            .addInt("RL_TEXTURE", RL_TEXTURE)

            // Culling
            .addInt("RL_CULL_FACE_FRONT", RL_CULL_FACE_FRONT)
            .addInt("RL_CULL_FACE_BACK", RL_CULL_FACE_BACK)

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
            .addInt("RL_BLEND_ALPHA", RL_BLEND_ALPHA)
            .addInt("RL_BLEND_ADDITIVE", RL_BLEND_ADDITIVE)
            .addInt("RL_BLEND_MULTIPLIED", RL_BLEND_MULTIPLIED)
            .addInt("RL_BLEND_ADD_COLORS", RL_BLEND_ADD_COLORS)
            .addInt("RL_BLEND_SUBTRACT_COLORS", RL_BLEND_SUBTRACT_COLORS)
            .addInt("RL_BLEND_ALPHA_PREMULTIPLY", RL_BLEND_ALPHA_PREMULTIPLY)
            .addInt("RL_BLEND_CUSTOM", RL_BLEND_CUSTOM)

            // Attachments (COMPLETO)
            .addInt("RL_ATTACHMENT_COLOR_CHANNEL0", RL_ATTACHMENT_COLOR_CHANNEL0)
            .addInt("RL_ATTACHMENT_COLOR_CHANNEL1", RL_ATTACHMENT_COLOR_CHANNEL1)
            .addInt("RL_ATTACHMENT_COLOR_CHANNEL2", RL_ATTACHMENT_COLOR_CHANNEL2)
            .addInt("RL_ATTACHMENT_DEPTH", RL_ATTACHMENT_DEPTH)
            .addInt("RL_ATTACHMENT_STENCIL", RL_ATTACHMENT_STENCIL)
            .addInt("RL_ATTACHMENT_TEXTURE2D", RL_ATTACHMENT_TEXTURE2D)
            .addInt("RL_ATTACHMENT_RENDERBUFFER", RL_ATTACHMENT_RENDERBUFFER)
            .addInt("RL_ATTACHMENT_CUBEMAP_POSITIVE_X", RL_ATTACHMENT_CUBEMAP_POSITIVE_X)
            .addInt("RL_ATTACHMENT_CUBEMAP_NEGATIVE_X", RL_ATTACHMENT_CUBEMAP_NEGATIVE_X)
            .addInt("RL_ATTACHMENT_CUBEMAP_POSITIVE_Y", RL_ATTACHMENT_CUBEMAP_POSITIVE_Y)
            .addInt("RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y", RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y)
            .addInt("RL_ATTACHMENT_CUBEMAP_POSITIVE_Z", RL_ATTACHMENT_CUBEMAP_POSITIVE_Z)
            .addInt("RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z", RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z)

            // Shader Types
            .addInt("RL_FRAGMENT_SHADER", RL_FRAGMENT_SHADER)
            .addInt("RL_VERTEX_SHADER", RL_VERTEX_SHADER)
            .addInt("RL_COMPUTE_SHADER", RL_COMPUTE_SHADER)
            
            // Uniform Types
            .addInt("RL_SHADER_UNIFORM_FLOAT", RL_SHADER_UNIFORM_FLOAT)
            .addInt("RL_SHADER_UNIFORM_VEC2", RL_SHADER_UNIFORM_VEC2)
            .addInt("RL_SHADER_UNIFORM_VEC3", RL_SHADER_UNIFORM_VEC3)
            .addInt("RL_SHADER_UNIFORM_VEC4", RL_SHADER_UNIFORM_VEC4)
            .addInt("RL_SHADER_UNIFORM_INT", RL_SHADER_UNIFORM_INT)
            .addInt("RL_SHADER_UNIFORM_IVEC2", RL_SHADER_UNIFORM_IVEC2)
            .addInt("RL_SHADER_UNIFORM_IVEC3", RL_SHADER_UNIFORM_IVEC3)
            .addInt("RL_SHADER_UNIFORM_IVEC4", RL_SHADER_UNIFORM_IVEC4)
            .addInt("RL_SHADER_UNIFORM_SAMPLER2D", RL_SHADER_UNIFORM_SAMPLER2D);
    }
}