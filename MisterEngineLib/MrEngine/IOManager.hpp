//
//  IOManager.hpp
//  Markine (Game Engine)
//
//  Created by Mark Griffith on 2016-07-15.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef IOManager_hpp
#define IOManager_hpp

#pragma once

#include <stdio.h>
#include <vector>
namespace MrEngine {
    
    
    class IOManager
    {
    public:
        static bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
        static bool readFileToBuffer(std::string filePath, std::string& buffer);

    };
    
    
}
#endif /* IOManager_hpp */
