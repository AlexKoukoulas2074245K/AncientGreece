///------------------------------------------------------------------------------------------------
///  OverworldMapPickingInfoSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldMapPickingInfoSystem.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../utils/OverworldUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/input/utils/InputUtils.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const glm::vec3 MAP_NORMAL = glm::vec3(0.0f, 0.0f, -1.0f);
    static const glm::vec3 MAP_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
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
    
    auto mapEntity = GetMapEntity();
    
    // Calculate render-constant camera view matrix
    auto viewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);

    // Calculate render-constant camera projection matrix
    auto projectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent.mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent.mZNear,
        cameraComponent.mZFar
    );
    
    // Calculate Mouse Intersection with map
    mapPickingInfoComponent.mMouseRayDirection = genesis::math::ComputeMouseRayDirection(viewMatrix, projectionMatrix, windowComponent.mRenderableWidth, windowComponent.mRenderableHeight);
    genesis::math::RayToPlaneIntersection(cameraComponent.mPosition, mapPickingInfoComponent.mMouseRayDirection, MAP_POSITION, MAP_NORMAL, mapPickingInfoComponent.mMapIntersectionPoint);
    mapPickingInfoComponent.mMapIntersectionPoint.z = genesis::rendering::GetTerrainHeightAtPosition(mapEntity, mapPickingInfoComponent.mMapIntersectionPoint);
    
    if (genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) == genesis::input::InputState::TAPPED)
    {
        Log(LogType::INFO, "Map position %.6f, %.6f, %.6f", mapPickingInfoComponent.mMapIntersectionPoint.x, mapPickingInfoComponent.mMapIntersectionPoint.y, mapPickingInfoComponent.mMapIntersectionPoint.z);
    }
}

///-----------------------------------------------------------------------------------------------

}
