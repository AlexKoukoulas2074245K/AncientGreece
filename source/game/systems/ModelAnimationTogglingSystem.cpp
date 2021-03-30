///------------------------------------------------------------------------------------------------
///  ModelAnimationTogglingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#include "ModelAnimationTogglingSystem.h"
#include "../GameContexts.h"
#include "../../engine/rendering/components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

ModelAnimationTogglingSystem::ModelAnimationTogglingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void ModelAnimationTogglingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>&  entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    for (const auto entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        renderableComponent.mShouldAnimateSkeleton = true;
        
        if (world.GetContext() == VIEW_CONTEXT && renderableComponent.mRenderableType != genesis::rendering::RenderableType::GUI_3D_MODEL)
        {
            renderableComponent.mShouldAnimateSkeleton = false;
        }
    }
}

///-----------------------------------------------------------------------------------------------
