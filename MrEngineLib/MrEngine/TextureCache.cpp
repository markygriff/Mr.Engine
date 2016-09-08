#include "TextureCache.hpp"
#include "ImageLoader.hpp"

#include <string>
#include <iostream>


namespace MrEngine {
    
    
    TextureCache::TextureCache()
    {
        //empty
    }

    TextureCache::~TextureCache()
    {
        //empty
    }

    GLTexture TextureCache::getTexture(std::string texturePath)
    {
        // lookup the texture and see if its in the map
        std::map<std::string, GLTexture>::iterator mit = mm_textureMap.find(texturePath);
        
        //auto mit = mm_textureMap.find(texturePath);
        
        //check if it's not in the map
        if (mit == mm_textureMap.end())
        {
            //load the texture
            GLTexture newTexture = ImageLoader::loadPNG(texturePath);
            
            //insert new texture into the map
            mm_textureMap.insert(make_pair(texturePath, newTexture));
            
            //std::cout << "Loaded Texture!" << std::endl;
            
            //return the newly loaded texture
            return newTexture;
        }
        
        //std::cout << "Loaded Cached Texture!" << std::endl;

        //else the texture has already been loaded, so return it
        return mit->second;
    }


}