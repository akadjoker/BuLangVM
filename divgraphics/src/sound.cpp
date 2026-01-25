#include "engine.hpp"

SoundLib gSoundLib;


void InitSound()
{
    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();
    }
}

void DestroySound()
{
    gSoundLib.destroy();
    CloseAudioDevice();
}

 

int SoundLib::load(const char *name, const char *soundPath)
{
    Sound snd = LoadSound(soundPath);
    SoundData sd;
    sd.id = (int)sounds.size();
    sd.sound = snd;
    strncpy(sd.name, name, MAXNAME - 1);
    sd.name[MAXNAME - 1] = '\0';

    sounds.push_back(sd);

    return sd.id;
}

Sound *SoundLib::getSound(int id)
{
    if (id < 0 || id >= (int)sounds.size())
        return nullptr;
    return &sounds[id].sound;
}

SoundData *SoundLib::getSoundData(int id)
{
    if (id < 0 || id >= (int)sounds.size())
        return nullptr;
    return &sounds[id];
}

void SoundLib::play(int id, float volume, float pitch)
{
    if (id < 0 || id >= (int)sounds.size())
        return;

    Sound *snd = getSound(id);
    if (!snd)
        return;

    SetSoundVolume(*snd, volume);
    SetSoundPitch(*snd, pitch);
    PlaySound(*snd);
}

void SoundLib::stop(int id)
{
    if (id < 0 || id >= (int)sounds.size())
        return;
    StopSound(sounds[id].sound);
}

void SoundLib::pause(int id)
{
    if (id < 0 || id >= (int)sounds.size())
        return;
    PauseSound(sounds[id].sound);
}

void SoundLib::resume(int id)
{
    if (id < 0 || id >= (int)sounds.size())
        return;
    ResumeSound(sounds[id].sound);
}

bool SoundLib::isSoundPlaying(int id)
{
    if (id < 0 || id >= (int)sounds.size())
        return false;
    return IsSoundPlaying(sounds[id].sound);
}

void SoundLib::destroy()
{
    for (size_t i = 0; i < sounds.size(); i++)
    {
        UnloadSound(sounds[i].sound);
    }
    sounds.clear();
}