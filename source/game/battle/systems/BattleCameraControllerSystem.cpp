///------------------------------------------------------------------------------------------------
///  BattleCameraControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleCameraControllerSystem.h"
#include "../utils/BattleUtils.h"
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

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float CAMERA_PANNING_SPEED             = 0.2f;
    static const float CAMERA_ZOOM_SPEED                = 1.0f;
    static const float CAMERA_ZOOM_SPEED_DECELERATION   = 9.8f;
    static const float CAMERA_MAX_Z                     = -0.2f;
    static const float CAMERA_MIN_Z                     = -0.4f;
    static const float CAMERA_AUTOCENTERING_SPEED       = 0.3f;
    static const float CAMERA_AUTOCENTERING_RESET_DELAY = 4.0f;
    static const float CAMERA_AUTOCENTERING_Y_OFFSET    = -0.35f;
}

///-----------------------------------------------------------------------------------------------

BattleCameraControllerSystem::BattleCameraControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleCameraControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const
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

void BattleCameraControllerSystem::NormalCameraOperation(const float dt) const
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
    
    if (glm::length(cameraComponent.mVelocity) > genesis::math::EQ_THRESHOLD)
    {
        cameraComponent.mDtAccum = 0.0f;
    }
    
    cameraComponent.mDtAccum += dt;
    if (cameraComponent.mDtAccum > CAMERA_AUTOCENTERING_RESET_DELAY)
    {
        cameraComponent.mDtAccum = 0.0f;
        cameraComponent.mCameraState = genesis::rendering::CameraState::AUTO_CENTERING;
    }
    
    
    // Auto center to middle of battlefield
    if (cameraComponent.mCameraState == genesis::rendering::CameraState::AUTO_CENTERING)
    {
        const auto& units = GetAllBattleUnitEntities();
        glm::vec3 positionCounter(0.0f);
        
        for (const auto entity: units)
        {
            const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entity);
            positionCounter += transformComponent.mPosition;
        }
        positionCounter /= units.size();
        positionCounter.y += CAMERA_AUTOCENTERING_Y_OFFSET;
        
        // If we havent already reached the player move towards them
        if (genesis::math::Abs(positionCounter.x - cameraComponent.mPosition.x) > genesis::math::EQ_THRESHOLD && genesis::math::Abs(positionCounter.y - cameraComponent.mPosition.y) > genesis::math::EQ_THRESHOLD)
        {
            const auto directionToPlayer = glm::normalize(positionCounter - cameraComponent.mPosition);
            cameraComponent.mVelocity.x = directionToPlayer.x * CAMERA_AUTOCENTERING_SPEED;
            cameraComponent.mVelocity.y = directionToPlayer.y * CAMERA_AUTOCENTERING_SPEED;
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
    
    if (exceededZoomLimits)
    {
        cameraComponent.mPosition = oldCameraPosition;
    }
}

///-----------------------------------------------------------------------------------------------

void BattleCameraControllerSystem::FreeCameraDebugOperation(const float dt) const
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
        cameraComponent.mRoll += dt;
        if (cameraComponent.mRoll >= 2 * genesis::math::PI)
        {
            cameraComponent.mRoll = cameraComponent.mRoll - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::RIGHT_ARROW_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mRoll -= dt;
        if (cameraComponent.mRoll <= 0.0f)
        {
            cameraComponent.mRoll = 2 * genesis::math::PI + cameraComponent.mRoll;
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

}
