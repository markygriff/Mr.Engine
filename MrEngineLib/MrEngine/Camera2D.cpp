#include "Camera2D.hpp"

#include <iostream>

namespace MrEngine
{
    
    
    Camera2D::Camera2D() :
        m_position(0.0f, 0.0f),
        m_cameraMatrix(1.0f), //4x4 identitiy matrix
        m_orthoMatrix(1.0f), //4x4 identity matrix
        m_scale(1.0f),
        m_needsMatrixUpdate(true),
        m_screenWidth(640),
        m_screenHeight(480)
    {
        //empty
    }

    Camera2D::~Camera2D()
    {
        //empty
    }

    void Camera2D::init(int screenWidth, int screenHeight)
    {
        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;
        m_orthoMatrix = glm::ortho(0.0f, (float)m_screenWidth, 0.0f, (float)m_screenHeight);

    }

    void Camera2D::update()
    {
        if (m_needsMatrixUpdate)
        {
            //where we translate the camera to
            glm::vec3 translate(-m_position.x + m_screenWidth/2, -m_position.y + m_screenHeight/2, 0.0f);
            //update camera position to translated position
            m_cameraMatrix = glm::translate(m_orthoMatrix, translate);
            
            //our new scale factor
            glm::vec3 scale(m_scale, m_scale, 0.0f);
            
            //update camera scale to updated scale (scale according to centre screen focal point)
            m_cameraMatrix = glm::scale(glm::mat4(1.0f), scale) * m_cameraMatrix;
            
            //no longer need to update
            m_needsMatrixUpdate = false;

        }
    }
    
    glm::vec2 Camera2D::convertScreenToWorld(glm::vec2 screenCoords)
    {
        //invert y direction
        screenCoords.y = m_screenHeight - screenCoords.y;
        //set 0 coordinate as the centre of the window
        screenCoords -= glm::vec2(m_screenWidth/2, m_screenHeight/2);
        //account for scale
        screenCoords /= m_scale;
        //translate with tthe camera position
        screenCoords += m_position;
        
        return screenCoords;
    }
    
    bool Camera2D::isBoxInView(const glm::vec2 &position, const glm::vec2 &dimensions)
    {
        
        glm::vec2 scaledScreenDimensions = glm::vec2(m_screenWidth, m_screenHeight) / (m_scale);
        
        //minimum distance the object can be away from the camera
        const float MIN_DISTANCE_X = dimensions.x / 2.0f + scaledScreenDimensions.x / 2.0f;
        const float MIN_DISTANCE_Y = dimensions.y / 2.0f + scaledScreenDimensions.y / 2.0f;
        
        //center position of object
        glm::vec2 centerPos = position + dimensions / 2.0f;
        //center position of camera
        glm::vec2 centerCameraPos = m_position;
        
        //vector from the centers of input to camera
        glm::vec2 distVec = centerPos - centerCameraPos;
        
        float xDepth = MIN_DISTANCE_X - std::abs(distVec.x);
        float yDepth = MIN_DISTANCE_Y - std::abs(distVec.y);
        
        //if true, there is a collision
        if (xDepth > 0 && yDepth > 0)
        {
            //collision
            return true;
        }
        return false;
    }
}
