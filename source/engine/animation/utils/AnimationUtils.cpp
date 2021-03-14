///------------------------------------------------------------------------------------------------
///  AnimationUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 13/03/2021.
///-----------------------------------------------------------------------------------------------

#include "AnimationUtils.h"
#include "../../rendering/components/RenderableComponent.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace animation
{

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

}

}
                     
