///------------------------------------------------------------------------------------------------
///  AnimationUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 13/03/2021.
///-----------------------------------------------------------------------------------------------

#include "AnimationUtils.h"
#include "../../common/utils/MathUtils.h"
#include "../../rendering/components/RenderableComponent.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace animation
{

///------------------------------------------------------------------------------------------------

void SetAnimation(const ecs::EntityId entityId, const StringId animationName, const bool randomizeStartTime /* false */)
{
    auto& world = ecs::World::GetInstance();
    auto& renderableComponent = world.GetComponent<rendering::RenderableComponent>(entityId);
    
    if (renderableComponent.mCurrentMeshResourceIndex != renderableComponent.mAnimNameToMeshIndex.at(animationName))
    {
        renderableComponent.mCurrentMeshResourceIndex = renderableComponent.mAnimNameToMeshIndex.at(animationName);
        renderableComponent.mAnimationTimeAccum = randomizeStartTime ? genesis::math::RandomFloat() : 0.0f;
    }
}

///------------------------------------------------------------------------------------------------

void ChangeAnimation(const ecs::EntityId entityId, const StringId animationName)
{
    auto& world = ecs::World::GetInstance();
    auto& renderableComponent = world.GetComponent<rendering::RenderableComponent>(entityId);
    
    if (renderableComponent.mCurrentMeshResourceIndex != renderableComponent.mAnimNameToMeshIndex.at(animationName))
    {
        renderableComponent.mPreviousMeshResourceIndex = renderableComponent.mCurrentMeshResourceIndex;
        renderableComponent.mCurrentMeshResourceIndex = renderableComponent.mAnimNameToMeshIndex.at(animationName);
        renderableComponent.mTransitionAnimationTimeAccum = 0.0f;
    }
}

///-----------------------------------------------------------------------------------------------)

int GetAnimationIndex(const ecs::EntityId entityId, const StringId animationName)
{
    const auto& world = ecs::World::GetInstance();
    const auto& renderableComponent = world.GetComponent<rendering::RenderableComponent>(entityId);
    
    if (renderableComponent.mAnimNameToMeshIndex.count(animationName))
    {
        return renderableComponent.mAnimNameToMeshIndex.at(animationName);
    }
    
    return -1;
}

///-----------------------------------------------------------------------------------------------)

}

}
                     
