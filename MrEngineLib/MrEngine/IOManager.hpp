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
#include <string>
#include <vector>

namespace MrEngine {
    
    struct DirEntry
    {
        std::string path;
        bool isDirectory;
    };
    
    class IOManager
    {
    public:
        static bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
        static bool readFileToBuffer(std::string filePath, std::string& buffer);
        
        /// Gets all directory entries of specified directory path and stores them in rvEntries
        /// Retruns false if path is not a directory
        static bool getDirectoryEntries(const char* path, std::vector<DirEntry>& rvEntries);
        static bool makeDirectory(const char* path);
    };
    
    
}
#endif /* IOManager_hpp */
