///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetSelectionSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 05/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldPlayerTargetSelectionSystem.h"
#include "../components/HighlightableComponent.h"
#include "../components/OverworldTargetComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/input/utils/InputUtils.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
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

namespace
{
    static const StringId MAP_ENTITY_NAME    = StringId("map");
    static const StringId PLAYER_ENTITY_NAME = StringId("player");
    
    static const std::string NAVMAP_ASSET_PATH = genesis::resources::ResourceLoadingService::RES_TEXTURES_ROOT + "nav_map.png";

    static const glm::vec3 MAP_NORMAL = glm::vec3(0.0f, 0.0f, -1.0f);
}

///-----------------------------------------------------------------------------------------------

OverworldPlayerTargetSelectionSystem::OverworldPlayerTargetSelectionSystem()
    : BaseSystem()
{
    genesis::resources::ResourceLoadingService::GetInstance().LoadResource(NAVMAP_ASSET_PATH);
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetSelectionSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    if (genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) != genesis::input::InputState::TAPPED)
    {
        return;
    }
    
    auto& world = genesis::ecs::World::GetInstance();
    
    auto playerEntity = world.FindEntityWithName(PLAYER_ENTITY_NAME);
    auto targetComponent = std::make_unique<OverworldTargetComponent>();
    auto entityToFollow = GetEntityToFollow(entitiesToProcess, playerEntity, world);
    
    if (entityToFollow != genesis::ecs::NULL_ENTITY_ID)
    {
        targetComponent->mOptionalEntityTarget = entityToFollow;
    }
    else
    {
        CalculateMapTarget(*targetComponent, world);
    }
    
    if (world.HasComponent<OverworldTargetComponent>(playerEntity))
    {
        world.RemoveComponent<OverworldTargetComponent>(playerEntity);
    }
    world.AddComponent<OverworldTargetComponent>(playerEntity, std::move(targetComponent));

}

///-----------------------------------------------------------------------------------------------

genesis::ecs::EntityId OverworldPlayerTargetSelectionSystem::GetEntityToFollow(const std::vector<genesis::ecs::EntityId>& entitiesToProcess, const genesis::ecs::EntityId playerEntity, genesis::ecs::World& world) const
{
    for (const auto entityId: entitiesToProcess)
    {
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
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    
    // Calculate render-constant camera view matrix
    auto mViewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);

    // Calculate render-constant camera projection matrix
    auto mProjectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent.mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent.mZNear,
        cameraComponent.mZFar
    );
    
    // Find map entity
    auto mapEntity = world.FindEntityWithName(MAP_ENTITY_NAME);
    const auto& mapTransformComponent = world.GetComponent<genesis::TransformComponent>(mapEntity);
    const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(mapEntity);
    const auto& mapMeshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
    const auto& mapDimensions = mapMeshResource.GetDimensions();
    
    // Calculate Mouse Intersection with map
    glm::vec3 mapIntersectionPoint;
    auto rayDirection = genesis::math::ComputeMouseRayDirection(mViewMatrix, mProjectionMatrix, windowComponent.mRenderableWidth, windowComponent.mRenderableHeight);
    genesis::math::RayToPlaneIntersection(cameraComponent.mPosition, rayDirection, mapTransformComponent.mPosition, MAP_NORMAL, mapIntersectionPoint);
    
    // Find relative displacement
    const auto relativeXDisplacement = mapIntersectionPoint.x/(mapDimensions.x/2.0f);
    const auto relativeYDisplacement = mapIntersectionPoint.y/(mapDimensions.y/2.0f);
    
    // Calculate respective navmap pixel
    auto& navmapTexture = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::TextureResource>(NAVMAP_ASSET_PATH);
    const auto targetPixelX = navmapTexture.GetWidth()/2 + relativeXDisplacement * navmapTexture.GetWidth()/2;
    const auto targetPixelY = navmapTexture.GetHeight()/2 - relativeYDisplacement * navmapTexture.GetHeight()/2;
    const auto targetPixel = navmapTexture.GetRGBatPixel(targetPixelX, targetPixelY);
    
    // Attach waypoint component to player
    targetComponent.mTargetPosition = mapIntersectionPoint;
    targetComponent.mTargetAreaType = RGB_TO_AREA_TYPE.count(targetPixel) > 0 ? RGB_TO_AREA_TYPE.at(targetPixel) : areaTypeMasks::NEUTRAL;
}

///-----------------------------------------------------------------------------------------------

}
