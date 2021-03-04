///------------------------------------------------------------------------------------------------
///  OverworldCameraControllerSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 04/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldCameraControllerSystem.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
#include "../../../engine/rendering/utils/CameraUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/input/utils/InputUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    const float ZERO_THRESHOLD                        = 0.01f;
    const float CAMERA_MOVEMENT_WINDOW_EDGE_THRESHOLD = 0.05f;
    const float CAMERA_MOVE_SPEED                     = 0.1f;
    const float CAMERA_ZOOM_SPEED                     = 1.0f;
    const float CAMERA_ZOOM_SPEED_DECELERATION        = 0.98f;
    const float CAMERA_MAX_Z                          = -0.10f;
    const float CAMERA_MIN_Z                          = -0.60f;
}

///-----------------------------------------------------------------------------------------------

OverworldCameraControllerSystem::OverworldCameraControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldCameraControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    
    const auto& mouseCoords = genesis::input::GetMousePosition();
    const auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    const auto oldCameraPosition = cameraComponent.mPosition;
    
    // Panning Calculations
    if(mouseCoords.x < windowComponent.mRenderableWidth * CAMERA_MOVEMENT_WINDOW_EDGE_THRESHOLD)
    {
        cameraComponent.mVelocity.x = -CAMERA_MOVE_SPEED;
    }
    if(mouseCoords.x > windowComponent.mRenderableWidth - windowComponent.mRenderableWidth * CAMERA_MOVEMENT_WINDOW_EDGE_THRESHOLD)
    {
        cameraComponent.mVelocity.x = CAMERA_MOVE_SPEED;
    }
    if (mouseCoords.y < windowComponent.mRenderableHeight * CAMERA_MOVEMENT_WINDOW_EDGE_THRESHOLD)
    {
        cameraComponent.mVelocity.y = CAMERA_MOVE_SPEED;
    }
    if (mouseCoords.y > windowComponent.mRenderableHeight - windowComponent.mRenderableWidth * CAMERA_MOVEMENT_WINDOW_EDGE_THRESHOLD)
    {
        cameraComponent.mVelocity.y = -CAMERA_MOVE_SPEED;
    }
    
    // Zoom Calculations
    if (genesis::input::GetMouseWheelDelta() > 0)
    {
        cameraComponent.mVelocity.z = CAMERA_ZOOM_SPEED;
    }
    else if (genesis::input::GetMouseWheelDelta() < 0)
    {
        cameraComponent.mVelocity.z = -CAMERA_ZOOM_SPEED;
    }
    
    cameraComponent.mPosition += cameraComponent.mVelocity * dt;
    cameraComponent.mVelocity = cameraComponent.mVelocity * CAMERA_ZOOM_SPEED_DECELERATION;
    if (glm::length(cameraComponent.mVelocity) < ZERO_THRESHOLD) {
        cameraComponent.mVelocity = glm::vec3();
    }
    
    // Check for exceeding limits
    auto exceededZoomLimits = cameraComponent.mPosition.z < CAMERA_MIN_Z || cameraComponent.mPosition.z > CAMERA_MAX_Z;
    auto exceededBounds = IsCameraOutOfBounds();
    
    if (exceededZoomLimits || exceededBounds)
    {
        cameraComponent.mPosition = oldCameraPosition;
    }
    
    // Establish front vector
    cameraComponent.mFrontVector.x = genesis::math::Cosf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.y = genesis::math::Sinf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.z = genesis::math::Sinf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
}

///-----------------------------------------------------------------------------------------------

bool OverworldCameraControllerSystem::IsCameraOutOfBounds() const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    const auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    
    // Temporary view & proj matrix for frustum calculations
    auto viewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);
    auto projectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent.mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent.mZNear,
        cameraComponent.mZFar
    );

    for (int i = 1; i < 5; ++i)
    {
        // Find map edge entity
        auto entityId = world.FindEntityWithName(StringId("map_edge_" + std::to_string(i)));
        
        // Temporary frustum for visibility calculation
        auto frustum = genesis::rendering::CalculateCameraFrustum(viewMatrix, projectionMatrix);

        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        const auto& currentMesh = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);

        // Check whether any map edge is visible at this time
        if (genesis::math::IsMeshInsideFrustum(transformComponent.mPosition, transformComponent.mScale, currentMesh.GetDimensions(), frustum))
        {
            return true;
        }
    }
    
    return false;
}


///-----------------------------------------------------------------------------------------------

}
