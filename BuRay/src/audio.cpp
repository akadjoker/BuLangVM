#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // AUDIO DEVICE
    // ========================================

    static int native_InitAudioDevice(Interpreter *vm, int argc, Value *args)
    {
        InitAudioDevice();
        return 0;
    }

    static int native_CloseAudioDevice(Interpreter *vm, int argc, Value *args)
    {
        CloseAudioDevice();
        return 0;
    }

    // ========================================
    // SOUND
    // ========================================

    static int native_LoadSound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
            return 0;
        Sound snd = LoadSound(args[0].asStringChars());
        Sound *ptr = new Sound(snd);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_UnloadSound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Sound *snd = (Sound *)args[0].asPointer();
        UnloadSound(*snd);
        delete snd;
        return 0;
    }

    static int native_PlaySound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Sound *snd = (Sound *)args[0].asPointer();
        PlaySound(*snd);
        return 0;
    }

    static int native_StopSound(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Sound *snd = (Sound *)args[0].asPointer();
        StopSound(*snd);
        return 0;
    }

    static int native_IsSoundPlaying(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Sound *snd = (Sound *)args[0].asPointer();
        vm->pushBool(IsSoundPlaying(*snd));
        return 1;
    }

    static int native_SetSoundVolume(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
            return 0;
        Sound *snd = (Sound *)args[0].asPointer();
        float volume = args[1].asDouble();
        SetSoundVolume(*snd, volume);
        return 0;
    }

    // ========================================
    // MUSIC
    // ========================================

    static int native_LoadMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
            return 0;
        Music music = LoadMusicStream(args[0].asStringChars());
        Music *ptr = new Music(music);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_UnloadMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Music *music = (Music *)args[0].asPointer();
        UnloadMusicStream(*music);
        delete music;
        return 0;
    }

    static int native_PlayMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Music *music = (Music *)args[0].asPointer();
        PlayMusicStream(*music);
        return 0;
    }

    static int native_UpdateMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Music *music = (Music *)args[0].asPointer();
        UpdateMusicStream(*music);
        return 0;
    }

    static int native_StopMusicStream(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Music *music = (Music *)args[0].asPointer();
        StopMusicStream(*music);
        return 0;
    }

    static int native_SetMusicVolume(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
            return 0;
        Music *music = (Music *)args[0].asPointer();
        float volume = args[1].asDouble();
        SetMusicVolume(*music, volume);
        return 0;
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
