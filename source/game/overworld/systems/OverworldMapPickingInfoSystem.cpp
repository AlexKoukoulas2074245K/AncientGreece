///------------------------------------------------------------------------------------------------
///  OverworldMapPickingInfoSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldMapPickingInfoSystem.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId MAP_ENTITY_NAME = StringId("map");
    static const glm::vec3 MAP_NORMAL = glm::vec3(0.0f, 0.0f, -1.0f);
}

///-----------------------------------------------------------------------------------------------

OverworldMapPickingInfoSystem::OverworldMapPickingInfoSystem()
    : BaseSystem()
{
    genesis::ecs::World::GetInstance().SetSingletonComponent<OverworldMapPickingInfoSingletonComponent>(std::make_unique<OverworldMapPickingInfoSingletonComponent>());
}

///-----------------------------------------------------------------------------------------------

void OverworldMapPickingInfoSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>&) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& mapPickingInfoComponent = world.GetSingletonComponent<OverworldMapPickingInfoSingletonComponent>();
    
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
    
    // Calculate Mouse Intersection with map
    mapPickingInfoComponent.mMouseRayDirection = genesis::math::ComputeMouseRayDirection(mViewMatrix, mProjectionMatrix, windowComponent.mRenderableWidth, windowComponent.mRenderableHeight);
    genesis::math::RayToPlaneIntersection(cameraComponent.mPosition, mapPickingInfoComponent.mMouseRayDirection, mapTransformComponent.mPosition, MAP_NORMAL, mapPickingInfoComponent.mMapIntersectionPoint);
}

///-----------------------------------------------------------------------------------------------

}
