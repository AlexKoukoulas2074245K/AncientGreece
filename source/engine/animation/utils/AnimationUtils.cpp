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

StringId GetCurrentAnimationName(const ecs::EntityId entityId)
{
    const auto& world = ecs::World::GetInstance();
    const auto& renderableComponent = world.GetComponent<rendering::RenderableComponent>(entityId);
    for (const auto& animNameToMeshIndexPair: renderableComponent.mAnimNameToMeshIndex)
    {
        if (animNameToMeshIndexPair.second == renderableComponent.mCurrentMeshResourceIndex)
        {
            return animNameToMeshIndexPair.first;
        }
    }
    
    assert(false && "No anim is playing");
    return StringId();
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
                     
