//
//  LevelReaderWriter.hpp
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-31.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef LevelReaderWriter_hpp
#define LevelReaderWriter_hpp

#include "Player.hpp"
#include "Box.hpp"
#include "Light.h"

#include <stdio.h>
#include <string>

class LevelReaderWriter
{
    
public:
    static bool saveAsText(const std::string& filePath,
                           const Player& player,
                           const std::vector<Box>& boxes,
                           const std::vector<Light>& lights);
    
    static bool saveAsBinary(const std::string& filePath,
                             const Player& player,
                             const std::vector<Box>& boxes,
                             const std::vector<Light>& lights);
    
    static bool loadAsText(const std::string& filePath,
                           b2World* world,
                           Player& player,
                           std::vector<Box>& boxes,
                           std::vector<Light>& lights);
    static bool loadAsBinary(const std::string& filePath,
                             b2World* world, Player& player,
                             std::vector<Box>& boxes,
                             std::vector<Light>& lights);
    
private:
    static bool saveAsTextV0(const std::string& filePath,
                             const Player& player,
                             const std::vector<Box>& boxes,
                             const std::vector<Light>& lights);
    static bool loadAsTextV0(std::ifstream& file,
                             b2World* world, Player& player,
                             std::vector<Box>& boxes,
                             std::vector<Light>& lights);
};


#endif /* LevelReaderWriter_hpp */
