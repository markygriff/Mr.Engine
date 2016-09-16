#include "LevelReaderWriter.hpp"

#include <ResourceManager.hpp>

#include <iostream>
#include <fstream>

//new versions added it here
const unsigned int TEXT_VERSION_0 = 100;

const unsigned int TEXT_VERSION = TEXT_VERSION_0;

bool LevelReaderWriter::saveAsText(const std::string& filePath,
                       const Player& player,
                       const std::vector<Box>& boxes,
                       const std::vector<Light>& lights)
{
    // Keep this updated with newest version
    return saveAsTextV0(filePath, player, boxes, lights);
}

bool LevelReaderWriter::saveAsTextV0(const std::string& filePath,
                                     const Player& player,
                                     const std::vector<Box>& boxes,
                                     const std::vector<Light>& lights)
{
    //open the file in the filePath dir
    std::ofstream file(filePath);
    if (file.fail())
    {
        perror(filePath.c_str());
        return false;
    }
    
    //write the version
    file << TEXT_VERSION << '\n';
    
    //write player data
    file << player.getPosition().x << ' ' << player.getPosition().y << ' '
    << player.getDrawDims().x << ' ' << player.getDrawDims().y << ' '
    << player.getCollisionDims().x << ' ' << player.getCollisionDims().y << ' '
    << player.getColor().r << ' ' << player.getColor().g << ' '
    << player.getColor().b << ' ' << player.getColor().a << '\n';
    
    //write number of boxes
    file << boxes.size() << '\n';
    //write all box data
    for (auto& b : boxes)
    {
        std::cout << "Box color: red: " << b.getColor().r << " green: "<< b.getColor().g << " blue: "<< b.getColor().b << std::endl;
        
        file << b.getPosition().x << ' ' << b.getPosition().y << ' '
        << b.getDimensions().x << ' ' << b.getDimensions().y << ' '
        << b.getColor().r << ' ' << b.getColor().g << ' '
        << b.getColor().b << ' ' << b.getColor().a << ' '
        << b.getUvRect().x << ' ' << b.getUvRect().y << ' '
        << b.getUvRect().z << ' ' << b.getUvRect().w << ' '
        << b.getAngle() << ' ' << b.getTexture().filePath << ' '
        << b.getIsDynamic() << ' ' << b.getFixedRotation() << '\n';
    }
    
    // Write number of lights
    file << lights.size() << '\n';
    // Write all lights
    for (auto& l : lights)
    {
        file << l.position.x << ' ' << l.position.y << ' '
        << l.size << ' ' << l.color.r << ' ' << l.color.g << ' '
        << l.color.b << ' ' << l.color.a << '\n';
    }
    
    return true;

}

bool LevelReaderWriter::saveAsBinary(const std::string& filePath,
                         const Player& player,
                         const std::vector<Box>& boxes,
                         const std::vector<Light>& lights)
{
    puts("saveBinary not yet implemented.");
    return false;
}

bool LevelReaderWriter::loadAsText(const std::string& filePath,
                       b2World* world,
                       Player& player,
                       std::vector<Box>& boxes,
                       std::vector<Light>& lights)
{
    //open file at filePath
    std::ifstream file(filePath);
    if (file.fail())
    {
        perror(filePath.c_str());
        return false;
    }
    
    //get version no.
    unsigned int version;
    file >> version;
    
    //load appropriate version
    switch (version)
    {
        case TEXT_VERSION_0:
            loadAsTextV0(file, world, player, boxes, lights);
            break;
        default:
            puts("Unknown version number in level file. File may be corrupted...");
            return false;
    }
    
    return true;
}

bool LevelReaderWriter::loadAsBinary(const std::string& filePath,
                         b2World* world, Player& player,
                         std::vector<Box>& boxes,
                         std::vector<Light>& lights)
{
    puts("loadBinary not yet implemented.");
    return false;
}



bool LevelReaderWriter::loadAsTextV0(std::ifstream& file,
                         b2World* world, Player& player,
                         std::vector<Box>& boxes,
                         std::vector<Light>& lights)
{
    { //read in player data
        glm::vec2 pos;
        glm::vec2 ddims;
        glm::vec2 cdims;
        MrEngine::ColorRGBA8 color;
        file >> pos.x >> pos.y >> ddims.x >> ddims.y >> cdims.x >> cdims.y >> color.r >> color.g >> color.b >> color.a;
        player.init(world, pos, ddims, cdims, color);
    }
    
    { //read in box data
        glm::vec2 pos;
        glm::vec2 dims;
        glm::vec4 uvRect;
        MrEngine::ColorRGBA8 color;
        bool fixedRotation;
        bool isDynamic;
        float angle;
        MrEngine::GLTexture texture;
        std::string texturePath;
        size_t numBoxes;
        
        //read the number of boxes
        file >> numBoxes;
        
        for (size_t i = 0; i < numBoxes; i++)
        {
            file >> pos.x >> pos.y >> dims.x >> dims.y >> color.r >> color.g >> color.b >> color.a
            >> uvRect.x >> uvRect.y >> uvRect.z >> uvRect.w
            >> angle >> texturePath >> isDynamic >> fixedRotation;
            
            texture = MrEngine::ResourceManager::getTexture(texturePath);
            
            //create box with loaded data
            boxes.emplace_back();
            boxes.back().init(world, pos, dims, texture, color, fixedRotation, isDynamic, angle, uvRect);
        }
    }
    
    { //read in lights
        glm::vec2 pos;
        float size;
        MrEngine::ColorRGBA8 color;
        size_t numLights;
        
        //read number of lights
        file >> numLights;
        
        for (size_t i = 0; i < numLights; i++)
        {
            file >> pos.x >> pos.y >> size >> color.r >> color.g >> color.b >> color.a;
            
            //create light with loaded data
            lights.emplace_back();
            lights.back().color = color;
            lights.back().position = pos;
            lights.back().size = size;
        }
    }
    return true;
}