//
//  IOManager.cpp
//  Markine (Game Engine)
//
//  Created by Mark Griffith on 2016-07-15.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#include "IOManager.hpp"
#include <fstream>

namespace MrEngine {
    
    
    bool IOManager::readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer)
    {
        //get ifstream of the file/
        std::ifstream file(filePath, std::ios::binary);
        
        if (file.fail())
        {
            perror(filePath.c_str());
            return false;
        }
        
        //seek to the end
        file.seekg(0, std::ios::end);
        //get the file size
        int fileSize = (int)file.tellg();
        //seek back to the beginning of the file
        file.seekg(0, std::ios::beg);
        
        //reduce the file size by any header bytes that may be present
        fileSize -= file.tellg();
        
        //resize our buffer to the right size
        buffer.resize(fileSize);
        
        //read bytes into the buffer
        //take 'fileSize' characters and store them in the array pointed to by '&(buffer[0])
        file.read((char *)&(buffer[0]), fileSize);
        file.close();
        
        return true;
    }
    
    bool IOManager::readFileToBuffer(std::string filePath, std::string& buffer)
    {
        //get ifstream of the file/
        std::ifstream file(filePath, std::ios::binary);
        
        if (file.fail())
        {
            perror(filePath.c_str());
            return false;
        }
        
        //seek to the end
        file.seekg(0, std::ios::end);
        //get the file size
        int fileSize = (int)file.tellg();
        //seek back to the beginning of the file
        file.seekg(0, std::ios::beg);
        
        //reduce the file size by any header bytes that may be present
        fileSize -= file.tellg();
        
        //resize our buffer to the right size
        buffer.resize(fileSize);
        
        //read bytes into the buffer
        //take 'fileSize' characters and store them in the array pointed to by '&(buffer[0])
        file.read((char *)&(buffer[0]), fileSize);
        file.close();
        
        return true;
    }
    
    
}

