///------------------------------------------------------------------------------------------------
///  BattleCameraControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleCameraControllerSystem.h"
#include "../components/BattleSideComponent.h"
#include "../components/BattleStateSingletonComponent.h"
#include "../utils/BattleUtils.h"
#include "../../overworld/utils/OverworldUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
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
    static const float CAMERA_PANNING_SPEED                  = 0.2f;
    static const float CAMERA_PANNING_CENTER_DISTANCE_FACTOR = 0.1f;
    static const float CAMERA_ZOOM_SPEED                     = 5.0f;
    static const float CAMERA_MAX_Z                          = -0.1f;
    static const float CAMERA_MIN_Z                          = -0.4f;
    static const float CAMERA_AUTOCENTERING_SPEED            = 0.3f;
    static const float CAMERA_AUTOCENTERING_Y_OFFSET         = -0.35f;
}

///-----------------------------------------------------------------------------------------------

BattleCameraControllerSystem::BattleCameraControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleCameraControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const
{
    if (IsBattleFinished()) return;
    
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
    const auto& battleStateComponent = world.GetSingletonComponent<BattleStateSingletonComponent>();
    
    const auto oldCameraPosition = cameraComponent.mPosition;
    
    cameraComponent.mVelocity.x = cameraComponent.mVelocity.y = 0.0f;
    
    // Calculate battlefield center
    const auto& units = GetAllBattleUnitEntities();
    const auto& playerUnitName = battleStateComponent.mPlayerUnitName;
    
    glm::vec3 battlefieldCenterPosition(cameraComponent.mPosition.z);
    auto unitsTakenIntoAccount = 0;
    for (const auto entity: units)
    {
        const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(entity);
        
        if (battleSideComponent.mBattleSideLeaderUnitName == playerUnitName || battleSideComponent.mBattleSideAssistingLeaderUnitName == playerUnitName)
        {
            const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entity);
            battlefieldCenterPosition += transformComponent.mPosition;
            unitsTakenIntoAccount++;
        }
    }
    battlefieldCenterPosition /= unitsTakenIntoAccount;
    battlefieldCenterPosition.y += CAMERA_AUTOCENTERING_Y_OFFSET;
    
    const auto battlefieldCenterDistance = genesis::math::Max(glm::distance(cameraComponent.mPosition, battlefieldCenterPosition), genesis::math::EQ_THRESHOLD);
    
    // Panning Calculations
    if(genesis::input::GetKeyState(genesis::input::Key::A_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.x = -CAMERA_PANNING_SPEED * (CAMERA_PANNING_CENTER_DISTANCE_FACTOR/battlefieldCenterDistance);
    }
    if(genesis::input::GetKeyState(genesis::input::Key::D_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.x = CAMERA_PANNING_SPEED * (CAMERA_PANNING_CENTER_DISTANCE_FACTOR/battlefieldCenterDistance);
    }
    if(genesis::input::GetKeyState(genesis::input::Key::W_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.y = CAMERA_PANNING_SPEED * (CAMERA_PANNING_CENTER_DISTANCE_FACTOR/battlefieldCenterDistance);
    }
    if(genesis::input::GetKeyState(genesis::input::Key::S_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mVelocity.y = -CAMERA_PANNING_SPEED * (CAMERA_PANNING_CENTER_DISTANCE_FACTOR/battlefieldCenterDistance);
    }
    
    if (glm::length(cameraComponent.mVelocity) > genesis::math::EQ_THRESHOLD)
    {
        cameraComponent.mDtAccum = 0.0f;
    }
    
    // Auto center to middle of battlefield
    if (cameraComponent.mCameraState == genesis::rendering::CameraState::AUTO_CENTERING)
    {
        // If we havent already reached the battlefield center, then move towards it
        if (battlefieldCenterDistance > genesis::math::EQ_THRESHOLD)
        {
            const auto directionToPlayer = glm::normalize(battlefieldCenterPosition - cameraComponent.mPosition);
            cameraComponent.mVelocity.x = directionToPlayer.x * CAMERA_AUTOCENTERING_SPEED;
            cameraComponent.mVelocity.y = directionToPlayer.y * CAMERA_AUTOCENTERING_SPEED;
        }
    }

    // Zoom Calculations
    if (genesis::input::GetMouseWheelDelta() > 0)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mPosition += glm::normalize(cameraComponent.mFrontVector) * CAMERA_ZOOM_SPEED * dt;
    }
    else if (genesis::input::GetMouseWheelDelta() < 0)
    {
        cameraComponent.mCameraState = genesis::rendering::CameraState::PANNING;
        cameraComponent.mPosition += glm::normalize(cameraComponent.mFrontVector)  * -CAMERA_ZOOM_SPEED * dt;
    }
    
    cameraComponent.mPosition += cameraComponent.mVelocity * dt;
    
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
