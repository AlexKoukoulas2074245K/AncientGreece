///------------------------------------------------------------------------------------------------
///  OverworldCameraControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 04/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldCameraControllerSystem.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/debug/components/DebugViewStateSingletonComponent.h"
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
    static const StringId PLAYER_ENTITY_NAME = StringId("player");

    static const float CAMERA_PANNING_SPEED           = 0.2f;
    static const float CAMERA_MOVE_TO_PLAYER_SPEED    = 1.0f;
    static const float CAMERA_ZOOM_SPEED              = 1.0f;
    static const float CAMERA_ZOOM_SPEED_DECELERATION = 9.8f;
    static const float CAMERA_MAX_Z                   = -0.2f;
    static const float CAMERA_MIN_Z                   = -0.6f;
}

///-----------------------------------------------------------------------------------------------

OverworldCameraControllerSystem::OverworldCameraControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldCameraControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& debugViewState = world.GetSingletonComponent<genesis::debug::DebugViewStateSingletonComponent>();
    
    if (debugViewState.mFreeCamDebugEnabled)
    {
        FreeCameraDebugOperation(dt);
    }
    else
    {
        NormalCameraOperation(dt);
    }
    
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    
    // Establish front vector
    cameraComponent.mFrontVector.x = genesis::math::Cosf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.y = genesis::math::Sinf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.z = genesis::math::Sinf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
}

///-----------------------------------------------------------------------------------------------

void OverworldCameraControllerSystem::NormalCameraOperation(const float dt) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    const auto oldCameraPosition = cameraComponent.mPosition;
    
    cameraComponent.mVelocity.x = cameraComponent.mVelocity.y = 0.0f;
    
    // Panning Calculations
    if(genesis::input::GetKeyState(genesis::input::Key::A_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.x = -CAMERA_PANNING_SPEED;
    }
    if(genesis::input::GetKeyState(genesis::input::Key::D_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.x = CAMERA_PANNING_SPEED;
    }
    if(genesis::input::GetKeyState(genesis::input::Key::W_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.y = CAMERA_PANNING_SPEED;
    }
    if(genesis::input::GetKeyState(genesis::input::Key::S_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.y = -CAMERA_PANNING_SPEED;
    }
    
    if (cameraComponent.mCameraState == genesis::rendering::CameraState::AUTO_CENTERING)
    {
        const auto playerEntity = world.FindEntityWithName(PLAYER_ENTITY_NAME);
        const auto& playerPosition = world.GetComponent<genesis::TransformComponent>(playerEntity).mPosition;
        
        // If we havent already reached the player move towards them
        if (genesis::math::Abs(playerPosition.x - cameraComponent.mPosition.x) > genesis::math::EQ_THRESHOLD && genesis::math::Abs(playerPosition.y - cameraComponent.mPosition.y) > genesis::math::EQ_THRESHOLD)
        {
            const auto directionToPlayer = glm::normalize(playerPosition - cameraComponent.mPosition);
            cameraComponent.mVelocity.x = directionToPlayer.x * CAMERA_MOVE_TO_PLAYER_SPEED;
            cameraComponent.mVelocity.y = directionToPlayer.y * CAMERA_MOVE_TO_PLAYER_SPEED;
        }
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
    
    // Decelerate zoom-in velocity
    if (cameraComponent.mVelocity.z > 0)
    {
        cameraComponent.mVelocity.z -= CAMERA_ZOOM_SPEED_DECELERATION * dt;
    }
    if (cameraComponent.mVelocity.z < 0)
    {
        cameraComponent.mVelocity.z += CAMERA_ZOOM_SPEED_DECELERATION * dt;
    }
    
    if (genesis::math::Abs(cameraComponent.mVelocity.z) < CAMERA_ZOOM_SPEED_DECELERATION * dt)
    {
        cameraComponent.mVelocity.z = 0.0f;
    }
    
    // Check for exceeding limits
    auto exceededZoomLimits = cameraComponent.mPosition.z < CAMERA_MIN_Z || cameraComponent.mPosition.z > CAMERA_MAX_Z;
    auto exceededBounds = IsCameraOutOfBounds();
    
    if (exceededZoomLimits || exceededBounds)
    {
        cameraComponent.mPosition = oldCameraPosition;
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldCameraControllerSystem::FreeCameraDebugOperation(const float dt) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    
    if (genesis::input::GetKeyState(genesis::input::Key::UP_ARROW_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPitch += dt;
        if (cameraComponent.mPitch >= 2 * genesis::math::PI)
        {
            cameraComponent.mPitch = cameraComponent.mPitch - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::DOWN_ARROW_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPitch -= dt;
        if (cameraComponent.mPitch <= 0.0f)
        {
            cameraComponent.mPitch = 2 * genesis::math::PI + cameraComponent.mPitch;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::LEFT_ARROW_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mYaw += dt;
        if (cameraComponent.mYaw >= 2 * genesis::math::PI)
        {
            cameraComponent.mYaw = cameraComponent.mYaw - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::RIGHT_ARROW_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mYaw -= dt;
        if (cameraComponent.mYaw <= 0.0f)
        {
            cameraComponent.mYaw = 2 * genesis::math::PI + cameraComponent.mYaw;
        }
    }
    
    if (genesis::input::GetKeyState(genesis::input::Key::A_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition += dt * 0.5f * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::GetKeyState(genesis::input::Key::D_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition -= dt * 0.5f * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::GetKeyState(genesis::input::Key::W_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition += dt * 0.5f * cameraComponent.mFrontVector;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::S_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition -= dt * 0.5f * cameraComponent.mFrontVector;
    }
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
        
        if (entityId != genesis::ecs::NULL_ENTITY_ID)
        {
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
    }
    
    return false;
}


///-----------------------------------------------------------------------------------------------

}
