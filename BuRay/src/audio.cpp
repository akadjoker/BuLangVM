#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // AUDIO DEVICE
    // ========================================

    static Value native_InitAudioDevice(Interpreter *vm, int argc, Value *args)
    {
        InitAudioDevice();
        return vm->makeNil();
    }

    static Value native_CloseAudioDevice(Interpreter *vm, int argc, Value *args)
    {
        CloseAudioDevice();
        return vm->makeNil();
    }

    // ========================================
    // SOUND
    // ========================================

    static Value native_LoadSound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
            return vm->makeNil();
        Sound snd = LoadSound(args[0].asStringChars());
        Sound *ptr = new Sound(snd);
        return vm->makePointer(ptr);
    }

    static Value native_UnloadSound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Sound *snd = (Sound *)args[0].asPointer();
        UnloadSound(*snd);
        delete snd;
        return vm->makeNil();
    }

    static Value native_PlaySound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Sound *snd = (Sound *)args[0].asPointer();
        PlaySound(*snd);
        return vm->makeNil();
    }

    static Value native_StopSound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Sound *snd = (Sound *)args[0].asPointer();
        StopSound(*snd);
        return vm->makeNil();
    }

    static Value native_IsSoundPlaying(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeBool(false);
        Sound *snd = (Sound *)args[0].asPointer();
        return vm->makeBool(IsSoundPlaying(*snd));
    }

    static Value native_SetSoundVolume(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
            return vm->makeNil();
        Sound *snd = (Sound *)args[0].asPointer();
        float volume = args[1].asDouble();
        SetSoundVolume(*snd, volume);
        return vm->makeNil();
    }

    // ========================================
    // MUSIC
    // ========================================

    static Value native_LoadMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
            return vm->makeNil();
        Music music = LoadMusicStream(args[0].asStringChars());
        Music *ptr = new Music(music);
        return vm->makePointer(ptr);
    }

    static Value native_UnloadMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Music *music = (Music *)args[0].asPointer();
        UnloadMusicStream(*music);
        delete music;
        return vm->makeNil();
    }

    static Value native_PlayMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Music *music = (Music *)args[0].asPointer();
        PlayMusicStream(*music);
        return vm->makeNil();
    }

    static Value native_UpdateMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Music *music = (Music *)args[0].asPointer();
        UpdateMusicStream(*music);
        return vm->makeNil();
    }

    static Value native_StopMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Music *music = (Music *)args[0].asPointer();
        StopMusicStream(*music);
        return vm->makeNil();
    }

    static Value native_SetMusicVolume(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
            return vm->makeNil();
        Music *music = (Music *)args[0].asPointer();
        float volume = args[1].asDouble();
        SetMusicVolume(*music, volume);
        return vm->makeNil();
    }

    // ========================================
    // REGISTER AUDIO
    // ========================================

    void register_audio(ModuleBuilder &mod)
    {
        mod.addFunction("InitAudioDevice", native_InitAudioDevice, 0)
           .addFunction("CloseAudioDevice", native_CloseAudioDevice, 0)

        // Sound
           .addFunction("LoadSound", native_LoadSound, 1)
           .addFunction("UnloadSound", native_UnloadSound, 1)
           .addFunction("PlaySound", native_PlaySound, 1)
           .addFunction("StopSound", native_StopSound, 1)
           .addFunction("IsSoundPlaying", native_IsSoundPlaying, 1)
           .addFunction("SetSoundVolume", native_SetSoundVolume, 2)

        // Music
           .addFunction("LoadMusicStream", native_LoadMusicStream, 1)
           .addFunction("UnloadMusicStream", native_UnloadMusicStream, 1)
           .addFunction("PlayMusicStream", native_PlayMusicStream, 1)
           .addFunction("UpdateMusicStream", native_UpdateMusicStream, 1)
           .addFunction("StopMusicStream", native_StopMusicStream, 1)
           .addFunction("SetMusicVolume", native_SetMusicVolume, 2);
    }

}
