///------------------------------------------------------------------------------------------------
///  CameraSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef CameraSingletonComponent_h
#define CameraSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"

#include <array>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

namespace 
{
    constexpr unsigned int CAMERA_FRUSTUM_SIDES = 6;

    const glm::vec3 DEFAULT_CAMERA_POSITION     = glm::vec3(0.0f, 0.0f, -5.0f);
    const glm::vec3 DEFAULT_CAMERA_FRONT_VECTOR = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 DEFAULT_CAMERA_UP_VECTOR    = math::Y_AXIS;

    const float DEFAULT_CAMERA_FIELD_OF_VIEW = math::PI / 6.0f;
    const float DEFAULT_CAMERA_Z_NEAR        = 0.1f;
    const float DEFAULT_CAMERA_Z_FAR         = 100.0f;
    const float DEFAULT_CAMERA_PITCH         = 0.0f;
    const float DEFAULT_CAMERA_YAW           = -1.5f * math::PI;
}


///-----------------------------------------------------------------------------------------------

using CameraFrustum = std::array<glm::vec4, CAMERA_FRUSTUM_SIDES>;

///-----------------------------------------------------------------------------------------------

class CameraSingletonComponent final: public ecs::IComponent
{
public:
    CameraFrustum mFrustum;
    glm::mat4 mViewMatrix       = glm::mat4(1.0f);
    glm::mat4 mProjectionMatrix = glm::mat4(1.0f);    
    glm::vec3 mPosition         = DEFAULT_CAMERA_POSITION;    
    glm::vec3 mFrontVector      = DEFAULT_CAMERA_FRONT_VECTOR;
    glm::vec3 mUpVector         = DEFAULT_CAMERA_UP_VECTOR;
    float mFieldOfView          = DEFAULT_CAMERA_FIELD_OF_VIEW;
    float mZNear                = DEFAULT_CAMERA_Z_NEAR;
    float mZFar                 = DEFAULT_CAMERA_Z_FAR;
    float mPitch                = DEFAULT_CAMERA_PITCH;
    float mYaw                  = DEFAULT_CAMERA_YAW;
};

///-----------------------------------------------------------------------------------------------

}

}

#endif /* CameraSingletonComponent_h */
