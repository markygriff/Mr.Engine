#ifndef AudioEngine_hpp
#define AudioEngine_hpp

#pragma once

#include <SDL2_mixer/SDL_mixer.h>

#include <stdio.h>
#include <string>
#include <map>

namespace MrEngine {
    
    class SoundEffect
    {
    public:
        friend class AudioEngine; ///< AudioEngine class can now use SoundEffect's private vars
        
        ///plays the effect file
        ///if loops == -1, loop forever
        ///otherwise play it 'loops + 1' times
        void play(int loops = 0);
    
    private:
        Mix_Chunk* m_chunk = nullptr;
    };
    
    class Music
    {
    public:
        friend class AudioEngine; ///< AudioEngine class can now use Music's private vars
        
        ///plays the music file
        ///if loops == -1, loop forever
        ///otherwise play it 'loops' times
        void play(int loops = 1);
        
        static void pause();
        static void stop();
        static void resume();
        
    private:
        Mix_Music* m_music = nullptr;
    };
    
    
    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();
        
        void init();
        void destroy();
        
        SoundEffect loadSoundEffect(const std::string& filePath);
        Music loadMusic(const std::string& filePath);
        
    private:
        //caches for effects and musics
        std::map<std::string, Mix_Chunk*> m_effectMap;
        std::map<std::string, Mix_Music*> m_musicMap;
        
        bool m_isInitialized = false;
    };
    
    
}

#endif /* AudioEngine_hpp */
