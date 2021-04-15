///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetSelectionSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 05/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldPlayerTargetSelectionSystem.h"
#include "../AreaTypes.h"
#include "../components/HighlightableComponent.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../components/OverworldTargetComponent.h"
#include "../utils/OverworldUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/input/utils/InputUtils.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/TextStringComponent.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/TextureResource.h"
#include "../../../engine/scripting/components/ScriptComponent.h"

#include <map>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

OverworldPlayerTargetSelectionSystem::OverworldPlayerTargetSelectionSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetSelectionSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    if (genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) != genesis::input::InputState::TAPPED)
    {
        return;
    }
    
    auto& world = genesis::ecs::World::GetInstance();
    
    auto playerEntity = GetPlayerEntity();
    auto targetComponent = std::make_unique<OverworldTargetComponent>();
    auto entityToFollow = GetEntityToFollow(entitiesToProcess, playerEntity, world);
    auto isValidTarget = false;
    if (entityToFollow != genesis::ecs::NULL_ENTITY_ID)
    {
        targetComponent->mEntityTargetToFollow = entityToFollow;
        isValidTarget = true;
    }
    else
    {
        CalculateMapTarget(*targetComponent, world);
        const auto& playerStatsComponent = world.GetComponent<UnitStatsComponent>(playerEntity);
        isValidTarget = (playerStatsComponent.mStats.mNavigableAreaTypes & targetComponent->mTargetAreaType) != 0;
    }
    
    if (isValidTarget)
    {
        //Log(LogType::INFO, "%.6f, %.6f, %.6f", targetComponent->mTargetPosition.x, targetComponent->mTargetPosition.y, targetComponent->mTargetPosition.z);
        
        auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
        cameraComponent.mCameraState = genesis::rendering::CameraState::AUTO_CENTERING;
        
        if (world.HasComponent<OverworldTargetComponent>(playerEntity))
        {
            world.RemoveComponent<OverworldTargetComponent>(playerEntity);
        }
        world.AddComponent<OverworldTargetComponent>(playerEntity, std::move(targetComponent));
    }
}

///-----------------------------------------------------------------------------------------------

genesis::ecs::EntityId OverworldPlayerTargetSelectionSystem::GetEntityToFollow(const std::vector<genesis::ecs::EntityId>& entitiesToProcess, const genesis::ecs::EntityId playerEntity, genesis::ecs::World& world) const
{
    for (const auto entityId: entitiesToProcess)
    {
        // Don't follow highlighted location texts
        if (world.HasComponent<genesis::rendering::TextStringComponent>(entityId))
        {
            continue;
        }
        if (world.GetComponent<HighlightableComponent>(entityId).mHighlighted && playerEntity != entityId)
        {
            return entityId;
        }
    }
    
    return genesis::ecs::NULL_ENTITY_ID;
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetSelectionSystem::CalculateMapTarget(OverworldTargetComponent& targetComponent, genesis::ecs::World& world) const
{
    const auto& mapPickingInfoComponent = world.GetSingletonComponent<OverworldMapPickingInfoSingletonComponent>();
    
    // Attach waypoint component to player
    targetComponent.mTargetPosition = mapPickingInfoComponent.mMapIntersectionPoint;
    targetComponent.mTargetAreaType = areaTypeMasks::NEUTRAL;
}

///-----------------------------------------------------------------------------------------------

}
