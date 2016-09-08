#ifndef Timing_hpp
#define Timing_hpp

#pragma once

#include <stdio.h>

namespace MrEngine
{
    
    class FpsLimiter
    {
    public:
        FpsLimiter();
        
        void init(float maxFPS);
        
        /// Sets the max FPS
        void setMaxFPS(float maxFPS);
        
        /// Sets the starting ticks
        void begin();
        
        /// Adds delay if necessary, returns the current FPS
        float end();
    
    private:
        
        /// Calculates the current FPS
        void calculateFPS();
        
        float m_fps;
        float m_maxFPS;
        float m_frameTime;
        unsigned int m_startTicks;
        
    };
    
    


}

#endif /* Timing_hpp */
