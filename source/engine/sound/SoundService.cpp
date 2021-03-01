///------------------------------------------------------------------------------------------------
///  SoundService.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "SoundService.h"
#include "../resources/ResourceLoadingService.h"
#include "../common/utils/Logging.h"
#include "../common/utils/OSMessageBox.h"
#include "../resources/MusicResource.h"
#include "../resources/SfxResource.h"

#include <cassert>
#include <fstream>
#include <SDL_mixer.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------ 

namespace sound
{

///------------------------------------------------------------------------------------------------ 

namespace
{ 

///------------------------------------------------------------------------------------------------

const std::string MUSIC_FILE_EXTENSION = ".ogg";
const std::string SFX_FILE_EXTENSION   = ".wav";

const int SFX_CHANNEL_NUMBER               = 1;
const int SOUND_FREQUENCY                  = 44100;
const int HARDWARE_CHANNELS                = 2;
const int CHUNK_SIZE_IN_BYTES              = 1024;
const int FADE_OUT_DURATION_IN_MILISECONDS = 1000;

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

static void OnMusicFinishedHook()
{
    SoundService::GetInstance().OnMusicFinished();
}

///------------------------------------------------------------------------------------------------

SoundService& SoundService::GetInstance()
{
    static SoundService instance;
    return instance;
}

///------------------------------------------------------------------------------------------------

SoundService::~SoundService()
{
    Mix_HookMusicFinished(nullptr);
}

///------------------------------------------------------------------------------------------------

void SoundService::Initialize() const
{
    SDL_version mixerCompiledVersion;
    SDL_MIXER_VERSION(&mixerCompiledVersion);

    const auto* imgLinkedVersion = Mix_Linked_Version();

    const auto mixerMajorVersionConsistency = mixerCompiledVersion.major == imgLinkedVersion->major;
    const auto mixerMinorVersionConsistency = mixerCompiledVersion.minor == imgLinkedVersion->minor;
    const auto mixerPatchConsistency = mixerCompiledVersion.patch == imgLinkedVersion->patch;
    const auto mixerVersionConsistency = mixerMajorVersionConsistency && mixerMinorVersionConsistency && mixerPatchConsistency;

    const auto sdlMixerInitFlags = MIX_INIT_OGG;
    if (!mixerVersionConsistency || Mix_Init(sdlMixerInitFlags) != sdlMixerInitFlags)
    {
        ShowMessageBox(MessageBoxType::ERROR, "SDL_Mixer", "SDL_Mixer was not initialize properly");
    }

    if (Mix_OpenAudio(SOUND_FREQUENCY, MIX_DEFAULT_FORMAT, HARDWARE_CHANNELS, CHUNK_SIZE_IN_BYTES) < 0)
    {
        ShowMessageBox(MessageBoxType::ERROR, "SDL_Mixer", "SDL_Mixer was not initialize properly");
    }

    Log(LogType::INFO, "Successfully initialized SDL_Mixer version %d.%d.%d", mixerCompiledVersion.major, mixerCompiledVersion.minor, mixerCompiledVersion.patch);
}

///------------------------------------------------------------------------------------------------

void SoundService::PlaySfx(const StringId& sfxName, const bool overrideCurrentPlaying /* true */)
{
    auto& resourceLoadingService = resources::ResourceLoadingService::GetInstance();

    const auto sfxFilePath = resources::ResourceLoadingService::RES_SFX_ROOT + sfxName.GetString();
    auto sfxFilePathWithExtension = sfxFilePath + SFX_FILE_EXTENSION;

    if (resourceLoadingService.HasLoadedResource(sfxFilePathWithExtension) == false)
    {
        Log(LogType::WARNING, "Sfx file %s requested not preloaded", sfxFilePathWithExtension.c_str());
        resourceLoadingService.LoadResource(sfxFilePathWithExtension);
    }

    auto& sfxResource = resourceLoadingService.GetResource<resources::SfxResource>(sfxFilePathWithExtension);

    if (overrideCurrentPlaying || Mix_Playing(1) == false)
    {        
        Mix_PlayChannel(SFX_CHANNEL_NUMBER, sfxResource.GetSdlSfxHandle(), 0);                
    }    
}

///------------------------------------------------------------------------------------------------

void SoundService::PlayMusic(const StringId& musicTrackName, const bool fadeOutEnabled /* true */)
{    
    auto& resourceLoadingService = resources::ResourceLoadingService::GetInstance();

    const auto musicFilePath        = resources::ResourceLoadingService::RES_MUSIC_ROOT + musicTrackName.GetString();    
    auto musicFilePathWithExtension = musicFilePath + MUSIC_FILE_EXTENSION;    

    if (resourceLoadingService.HasLoadedResource(musicFilePathWithExtension) == false)
    {
        Log(LogType::WARNING, "Music file %s requested not preloaded", musicFilePathWithExtension.c_str());
        resourceLoadingService.LoadResource(musicFilePathWithExtension);
    }

    auto& musicResource = resourceLoadingService.GetResource<resources::MusicResource>(musicFilePathWithExtension);
    
    if (fadeOutEnabled == false)
    {        
        Mix_PlayMusic(musicResource.GetSdlMusicHandle(), -1);        
    }
    else
    {        
        mQueuedMusicResourceId = resourceLoadingService.GetResourceIdFromPath(musicFilePathWithExtension);
        Mix_FadeOutMusic(FADE_OUT_DURATION_IN_MILISECONDS);
        Mix_HookMusicFinished(OnMusicFinishedHook);
    }    
}

///------------------------------------------------------------------------------------------------

void SoundService::MuteMusic()
{
    if (!mAllAudioDisabled)
    {
        if (Mix_VolumeMusic(-1) > 0)
        {
            mMusicVolumePriorToMuting = Mix_VolumeMusic(-1);
        }
        
        Mix_VolumeMusic(0);
    }    
}

///------------------------------------------------------------------------------------------------

void SoundService::UnmuteMusic()
{
    if (!mAllAudioDisabled)
    {
        Mix_VolumeMusic(mMusicVolumePriorToMuting);
    }
}

///------------------------------------------------------------------------------------------------

void SoundService::MuteSfx()
{
    if (!mAllAudioDisabled)
    {
        mSfxVolumePriorToMuting = Mix_Volume(SFX_CHANNEL_NUMBER, -1);
        Mix_Volume(SFX_CHANNEL_NUMBER, 0);
    }    
}

///------------------------------------------------------------------------------------------------

void SoundService::UnmuteSfx()
{
    if (!mAllAudioDisabled)
    {
        Mix_Volume(SFX_CHANNEL_NUMBER, mMusicVolumePriorToMuting);
    }
}

///------------------------------------------------------------------------------------------------

void SoundService::ToggleAllAudioOnOff()
{
    mAllAudioDisabled = !mAllAudioDisabled;
    if (!mAllAudioDisabled)
    {
        Mix_VolumeMusic(mMusicVolumePriorToMuting);
        Mix_Volume(SFX_CHANNEL_NUMBER, mMusicVolumePriorToMuting);
    }
    else
    {
        mMusicVolumePriorToMuting = Mix_VolumeMusic(-1);
        Mix_VolumeMusic(0);
        mSfxVolumePriorToMuting = Mix_Volume(SFX_CHANNEL_NUMBER, -1);
        Mix_Volume(SFX_CHANNEL_NUMBER, 0);
    }
}

///------------------------------------------------------------------------------------------------

bool SoundService::IsPlayingMusic() const
{
    return Mix_PlayingMusic() != 0;
}

///------------------------------------------------------------------------------------------------

bool SoundService::IsPlayingSfx() const
{
    return Mix_Playing(SFX_CHANNEL_NUMBER) != 0;
}

///------------------------------------------------------------------------------------------------

void SoundService::OnMusicFinished()
{
    if (mQueuedMusicResourceId != 0)
    {
        auto& resourceLoadingService = resources::ResourceLoadingService::GetInstance();
        auto& musicResource = resourceLoadingService.GetResource<resources::MusicResource>(mQueuedMusicResourceId);

        Mix_PlayMusic(musicResource.GetSdlMusicHandle(), -1);
    }
}

///------------------------------------------------------------------------------------------------

void SoundService::OnSfxFinished()
{
    UnmuteMusic();
}

///------------------------------------------------------------------------------------------------

}

}