///------------------------------------------------------------------------------------------------
///  SoundService.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef SoundService_h
#define SoundService_h

///------------------------------------------------------------------------------------------------

#include "../common/utils/StringUtils.h"
#include "../../engine/GenesisEngine.h"

#include <functional>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace sound
{

///------------------------------------------------------------------------------------------------

using MusicResourceId = unsigned int;

///------------------------------------------------------------------------------------------------
/// A class providing access to sound playback utilities.
class SoundService final
{        
    friend class genesis::GenesisEngine;

public:
    /// The default method of getting a hold of this singleton.
    ///
    /// The single instance of this class will be lazily initialized
    /// the first time it is needed.
    /// @returns a reference to the single instance of this class.    
    static SoundService& GetInstance();    

    ~SoundService();
    SoundService(const SoundService&) = delete;
    SoundService(SoundService&&) = delete;
    const SoundService& operator = (const SoundService&) = delete;
    SoundService& operator = (SoundService&&) = delete;    
    
    /// Plays a sound effect with the given name. 
    ///
    /// @param[in] sfxName the name of the sound effect to play (that is the filename of the sfx without the extension).
    /// @param[in] overrideCurrentPlaying whether or not the currently playing sfx should stop playing and get overridden by the new one.
    void PlaySfx(const StringId& sfxName, const bool overrideCurrentPlaying = true);

    /// Plays a music track with the given name (this is the filename of the music track without the extension).
    ///
    /// @param[in] musicTrackName the name of the music track to play
    /// @param[in] fadeOutEnabled whether or not the currently playing music track should fade out giving way to the new one.
    void PlayMusic(const StringId& musicTrackName, const bool fadeOutEnabled = true);

    /// Mutes the volume of the music track channel.   
    void MuteMusic();

    /// Unmutes the volume of the music track channel.   
    void UnmuteMusic();    

    /// Mutes the volume of the sfx channel.   
    void MuteSfx();

    /// Unmutes the volume of the sfx channel.   
    void UnmuteSfx();

    /// Toggle all audio (music & sfx) on/off.    
    void ToggleAllAudioOnOff();    
    
    /// Check whether a music track is currently playing
    /// @returns whether the music channel currently has an active track playing
    bool IsPlayingMusic() const;

    /// Check whether a sound effect is currently playing
    /// @returns whether the sfx channel currently has an active sfx playing
    bool IsPlayingSfx() const;    
    
    /// To be used internally
    void OnMusicFinished();
    void OnSfxFinished();

private:    
    SoundService() = default;    
    
    void Initialize() const;

    MusicResourceId mQueuedMusicResourceId;
    int mMusicVolumePriorToMuting = 0;
    int mSfxVolumePriorToMuting = 0;
    bool mAllAudioDisabled = false;

};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* SoundService_h */
