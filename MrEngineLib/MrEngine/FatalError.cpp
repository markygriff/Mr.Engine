#include "FatalError.hpp"

#include <iostream>
#include <SDL2/SDL.h>


namespace MrEngine
{
    
    
    void fatalError(std::string error)
    {
        std::cout << error << std::endl;
        
        SDL_Quit();
        exit(80);
    }
    

}