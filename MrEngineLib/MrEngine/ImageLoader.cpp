#include "ImageLoader.hpp"
#include "picoPNG.hpp"
#include "IOManager.hpp"

#include "FatalError.hpp"

namespace MrEngine {
    
    
    GLTexture ImageLoader::loadPNG(std::string filePath)
    {
        // initialize texture object to 0
        GLTexture texture = {};

        std::vector<unsigned char> in;
        std::vector<unsigned char> out;

        unsigned long width;
        unsigned long height;

        if (IOManager::readFileToBuffer(filePath, in) == false)
        {
            fatalError("Failed to load PNG file to buffer!");
        }

        int errorCode = decodePNG(out, width, height, &(in[0]), in.size());

        if (errorCode != 0)
        {
            fatalError("decodePNG failed with error " + std::to_string(errorCode));
        }

        glGenTextures(1, &(texture.id));

        glBindTexture(GL_TEXTURE_2D, texture.id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        //generate the mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        //unibind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        //set texture dimensions
        texture.width = (int)width;
        texture.height = (int)height;
        texture.filePath = filePath;

        //return copy of the texture data
        return texture;

    }
    
    
}
