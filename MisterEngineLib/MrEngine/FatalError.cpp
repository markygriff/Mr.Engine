//
//  FatalError.cpp
//  MisterEngineLib
//
//  Created by Mark Griffith on 2016-08-01.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

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