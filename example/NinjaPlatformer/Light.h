//
//  Light.h
//  NinjaPlatformer
//
//  Created by Mark Griffith on 2016-08-10.
//  Copyright © 2016 Mark Griffith. All rights reserved.
//

#ifndef Light_h
#define Light_h

#include <Vertex.h>
#include <SpriteBatch.hpp>
#include <glm/glm.hpp>

//hacky 2D lighting effect
class Light
{
public:
    
    void draw(MrEngine::SpriteBatch& spriteBatch)
    {
        glm::vec4 destRect;
        destRect.x = position.x - size / 2.0f;
        destRect.y = position.y - size / 2.0f;
        destRect.z = size;
        destRect.w = size;
        
        //draw the batch of boxes
        spriteBatch.draw(destRect, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, 0.0f, color, 0.0f);
    }
    
    MrEngine::ColorRGBA8 color;
    glm::vec2 position;
    float size;

};


#endif /* Light_h */
