///------------------------------------------------------------------------------------------------
///  LightUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 27/07/2020.
///------------------------------------------------------------------------------------------------

#include "LightUtils.h"
#include "../components/LightStoreSingletonComponent.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

void AddLightSource
(
    const glm::vec3& lightPosition,
    const float lightPower
)
{
    auto& lightStoreComponent = ecs::World::GetInstance().GetSingletonComponent<LightStoreSingletonComponent>();
    lightStoreComponent.mLightPositions.emplace_back(lightPosition);
    lightStoreComponent.mLightPowers.emplace_back(lightPower);
}

///------------------------------------------------------------------------------------------------

}

}
