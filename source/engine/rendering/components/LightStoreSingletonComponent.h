///------------------------------------------------------------------------------------------------
///  LightStoreSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 27/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef LightStoreSingletonComponent_h
#define LightStoreSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class LightStoreSingletonComponent final: public ecs::IComponent
{
public:
    std::vector<glm::vec3> mLightPositions;
    std::vector<float> mLightPowers;
    glm::mat4 mMainShadowCastingLightMatrix;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------


#endif /* LightStoreSingletonComponent_h */
