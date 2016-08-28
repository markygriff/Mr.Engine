//
//  AudioEngine.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-01.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "AudioEngine.hpp"
#include "FatalError.hpp"
 

namespace MrEngine {
    
    
    void SoundEffect::play(int loops)
    {
        if (Mix_PlayChannel(-1, m_chunk, loops) == -1)
        {
            //in the case of rapid succession of played sound effects,
            //multiple channels will be needed as one channel cannot handle
            //many effects at once
            if (Mix_PlayChannel(0, m_chunk, loops) == -1) {
                fatalError("Mix_PlayChannel error: " + std::string(Mix_GetError()));
            }
        }
    }
    
    void Music::play(int loops)
    {
        if (Mix_PlayMusic(m_music, loops) == -1)
        {
            fatalError("Mix_PlayMusic error: " + std::string(Mix_GetError()));
            
        }
    }
    
    void Music::pause()
    {
        //pauses any music playing
        Mix_PauseMusic();
    }
    
    void Music::stop()
    {
        //terminates any music playing
        Mix_HaltMusic();
    }
    
    void Music::resume()
    {
        //resumes any paused music
        Mix_ResumeMusic();
    }
    
    AudioEngine::AudioEngine()
    {
        
    }
    
    AudioEngine::~AudioEngine()
    {
        destroy();
    }
    
    void AudioEngine::init()
    {
        if (m_isInitialized)
        {
            fatalError("AudioEngine Already Initialied.");
        }
        //parameter can be bitwise combination of MIX_INIT_FAC,
        // MIX_INIT_MOD, MIX_INIT_MP3, MIX_INIT_OGG
        if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1)
        {
            fatalError("Mix_Init error: " + std::string(Mix_GetError()));
        }
        
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        {
            fatalError("Mix_OpenAudio error: " + std::string(Mix_GetError()));

        }
        
        m_isInitialized = true;
    }
    
    void AudioEngine::destroy()
    {
        if (m_isInitialized == true)
        {
            m_isInitialized = false;
            
            //free effect chunks
            for (auto& it : m_effectMap)
            {
                Mix_FreeChunk(it.second);
            }
            
            //free music
            for (auto& it : m_musicMap)
            {
                Mix_FreeMusic(it.second);
            }
            
            m_effectMap.clear();
            m_musicMap.clear();
            
            Mix_CloseAudio();
            Mix_Quit();
        }
    }
    
    SoundEffect AudioEngine::loadSoundEffect(const std::string& filePath)
    {
        
        SoundEffect effect;
        
        //search for audio in cache
        auto it = m_effectMap.find(filePath);
        if (it == m_effectMap.end())
            {
                //did not find the audio, load it!
                Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
                
                //error checking
                if (chunk == nullptr)
                {
                    fatalError("Mix_LoadWAV error: " + std::string(Mix_GetError()));

                }
                
                effect.m_chunk = chunk;
                
                m_effectMap[filePath] = chunk;
            }
        else
        {
            //already been cached
            effect.m_chunk = it->second;
        }
    
        return effect;
    }
    
    Music AudioEngine::loadMusic(const std::string& filePath)
    {
        Music music;
        
        //search for audio in cache
        auto it = m_musicMap.find(filePath);
        if (it == m_musicMap.end())
        {
            //did not find the audio, load it!
            Mix_Music* mixMusic = Mix_LoadMUS(filePath.c_str());
            
            //error checking
            if (mixMusic == nullptr)
            {
                fatalError("Mix_LoadMUS error: " + std::string(Mix_GetError()));
                
            }
            
            music.m_music = mixMusic;
            
            m_musicMap[filePath] = mixMusic;
        }
        else
        {
            //already been cached
            music.m_music = it->second;
        }
        
        return music;
    }
    
}
