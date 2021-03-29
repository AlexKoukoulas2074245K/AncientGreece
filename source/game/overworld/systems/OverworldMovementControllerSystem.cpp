///------------------------------------------------------------------------------------------------
///  OverworldMovementControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldMovementControllerSystem.h"
#include "../AreaTypes.h"
#include "../components/OverworldTargetComponent.h"
#include "../utils/NavmapUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/TextureResource.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    const tsl::robin_map<AreaTypeMask, float> AREA_TYPE_TO_SPEED_MULTIPLIER =
    {
        { areaTypeMasks::FOREST, 0.5f },
        { areaTypeMasks::MOUNTAIN, 0.5f },
        { areaTypeMasks::HIGH_MOUNTAIN, 0.25f },
    };

    static const std::string NAVMAP_ASSET_PATH = genesis::resources::ResourceLoadingService::RES_TEXTURES_ROOT + "nav_map.png";

    static const StringId MAP_ENTITY_NAME = StringId("map");

    static const float SUFFICIENTLY_CLOSE_THRESHOLD = 0.001f;
    static const float ROTATION_SPEED               = 5.0f;
    static const float BASE_UNIT_SPEED              = 0.002f;
}

///-----------------------------------------------------------------------------------------------

OverworldMovementControllerSystem::OverworldMovementControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& navmapTexture = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::TextureResource>(NAVMAP_ASSET_PATH);
    
    auto mapEntity = world.FindEntityWithName(MAP_ENTITY_NAME);
    const auto& mapRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(mapEntity);
    const auto& mapMeshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( mapRenderableComponent.mMeshResourceIds.at(mapRenderableComponent.mCurrentMeshResourceIndex));
    const auto& mapDimensions = mapMeshResource.GetDimensions();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        auto& waypointComponent = world.GetComponent<OverworldTargetComponent>(entityId);
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        if (waypointComponent.mOptionalEntityTarget != genesis::ecs::NULL_ENTITY_ID && world.HasEntity(waypointComponent.mOptionalEntityTarget))
        {
            waypointComponent.mTargetPosition = world.GetComponent<genesis::TransformComponent>(waypointComponent.mOptionalEntityTarget).mPosition;
        }
        
        const auto& vecToWaypoint = waypointComponent.mTargetPosition - transformComponent.mPosition;
        
        // If we have arrived at target position
        if (glm::length(vecToWaypoint) < SUFFICIENTLY_CLOSE_THRESHOLD)
        {
            // Start Idle animation
            genesis::animation::ChangeAnimation(entityId, StringId("idle"));
            world.RemoveComponent<OverworldTargetComponent>(entityId);
        }
        // Else move and rotate towards target
        else
        {
            const auto terrainSpeedMultiplier = GetTerrainSpeedMultiplier(transformComponent.mPosition, mapDimensions, navmapTexture);
            const auto unitSpeed = BASE_UNIT_SPEED * unitStatsComponent.mSpeedMultiplier * terrainSpeedMultiplier;
            UpdatePosition(dt, unitSpeed, waypointComponent.mTargetPosition, transformComponent.mPosition);
            UpdateRotation(dt, -genesis::math::Arctan2(vecToWaypoint.x, vecToWaypoint.y), transformComponent.mRotation);
            
            // Start walking animation
            genesis::animation::ChangeAnimation(entityId, StringId("walking"));
        }
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::UpdatePosition(const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const
{
    const auto& movementDirection = glm::normalize(targetPosition - entityPosition);
    entityPosition += movementDirection * speed * dt;
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::UpdateRotation(const float dt, const float target, glm::vec3& entityRotation) const
{
    auto targetRotation = target;
    while (entityRotation.z - targetRotation > genesis::math::PI) targetRotation += genesis::math::PI * 2.0f;
    while (targetRotation - entityRotation.z > genesis::math::PI) entityRotation.z += genesis::math::PI * 2.0f;
    
    if (genesis::math::Abs(targetRotation - entityRotation.z) > ROTATION_SPEED * dt)
    {
        entityRotation.z = targetRotation > entityRotation.z ? entityRotation.z + ROTATION_SPEED * dt : entityRotation.z - ROTATION_SPEED * dt;
        
        if (entityRotation.z > genesis::math::PI * 2.0f)
        {
            entityRotation.z -= genesis::math::PI * 2.0f;
        }
        else if (entityRotation.z < -genesis::math::PI * 2.0f)
        {
            entityRotation.z += genesis::math::PI * 2.0f;
        }
    }
}

///-----------------------------------------------------------------------------------------------

float OverworldMovementControllerSystem::GetTerrainSpeedMultiplier(const glm::vec3& unitPosition, const glm::vec3& mapDimensions, const genesis::resources::TextureResource& navmapTexture) const
{
    auto terrainMultiplier = 1.0f;
    const auto pixelPosition = MapPositionToNavmapPixel(unitPosition, mapDimensions, navmapTexture.GetDimensions());
    const auto targetPixel = navmapTexture.GetRgbAtPixel(pixelPosition.x, pixelPosition.y);
    
    if (RGB_TO_AREA_TYPE_MASK.count(targetPixel))
    {
        const auto areaType = RGB_TO_AREA_TYPE_MASK.at(targetPixel);
        if (AREA_TYPE_TO_SPEED_MULTIPLIER.count(areaType))
        {
            terrainMultiplier = AREA_TYPE_TO_SPEED_MULTIPLIER.at(areaType);
        }
    }
    
    return terrainMultiplier;
}

///-----------------------------------------------------------------------------------------------

}
