///------------------------------------------------------------------------------------------------
///  Game.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "overworld/systems/OverworldCameraControllerSystem.h"
#include "overworld/systems/OverworldTargetSelectionSystem.h"
#include "../engine/ECS.h"
#include "../engine/common/components/TransformComponent.h"
#include "../engine/common/utils/Logging.h"
#include "../engine/common/utils/MathUtils.h"
#include "../engine/debug/components/DebugViewStateSingletonComponent.h"
#include "../engine/debug/systems/ConsoleManagementSystem.h"
#include "../engine/debug/systems/DebugViewManagementSystem.h"
#include "../engine/input/systems/RawInputHandlingSystem.h"
#include "../engine/input/utils/InputUtils.h"
#include "../engine/rendering/components/CameraSingletonComponent.h"
#include "../engine/rendering/components/LightStoreSingletonComponent.h"
#include "../engine/rendering/components/RenderableComponent.h"
#include "../engine/rendering/components/WindowSingletonComponent.h"
#include "../engine/rendering/utils/LightUtils.h"
#include "../engine/rendering/utils/MeshUtils.h"
#include "../engine/rendering/systems/BoneAnimationSystem.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/systems/ScriptingSystem.h"

///------------------------------------------------------------------------------------------------

void Game::VOnSystemsInit()
{
    auto& world = genesis::ecs::World::GetInstance();
    world.AddSystem(std::make_unique<genesis::input::RawInputHandlingSystem>());
    world.AddSystem(std::make_unique<genesis::scripting::ScriptingSystem>());

#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    world.AddSystem(std::make_unique<genesis::debug::ConsoleManagementSystem>());
    world.AddSystem(std::make_unique<genesis::debug::DebugViewManagementSystem>());
#endif
    
    world.AddSystem(std::make_unique<overworld::OverworldCameraControllerSystem>());
    //world.AddSystem(std::make_unique<overworld::OverworldTargetSelectionSystem>());
    
    world.AddSystem(std::make_unique<genesis::rendering::BoneAnimationSystem>());
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

///------------------------------------------------------------------------------------------------

void Game::VOnGameInit()
{
    genesis::rendering::LoadAndCreateModelByName("map", genesis::rendering::ModelType::OBJ, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", genesis::rendering::ModelType::OBJ, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_1"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", genesis::rendering::ModelType::OBJ, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_2"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", genesis::rendering::ModelType::OBJ, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_3"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", genesis::rendering::ModelType::OBJ, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_4"));
    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
    genesis::rendering::AddLightSource(glm::vec3(2.0f, 2.0f, 0.0f), 4.0f);
    
    genesis::rendering::LoadAndCreateModelByName("walking", genesis::rendering::ModelType::DAE, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.005f, 0.005f, 0.005f), StringId("spartan"));
}

///------------------------------------------------------------------------------------------------

static float dtAccum = 0.0f;

void Game::VOnUpdate(const float dt)
{
    auto& world = genesis::ecs::World::GetInstance();
    
    auto& lightStoreComponent = world.GetSingletonComponent<genesis::rendering::LightStoreSingletonComponent>();
    
    dtAccum += dt;
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(dtAccum/2) * 2;
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(dtAccum/2) * 2;
    auto moveSpeed = 0.5f;
    auto lookSpeed = 1.0f;
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    
    auto entity = world.FindEntityWithName(StringId("spartan"));
    if (genesis::input::GetKeyState(genesis::input::Key::R_KEY) == genesis::input::InputState::PRESSED)
    {
        world.GetComponent<genesis::TransformComponent>(entity).mPosition.z -= 0.1f * dt;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::T_KEY) == genesis::input::InputState::PRESSED)
    {
        world.GetComponent<genesis::TransformComponent>(entity).mPosition.z += 0.1f * dt;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::F_KEY) == genesis::input::InputState::PRESSED)
    {
        world.GetComponent<genesis::TransformComponent>(entity).mScale -= glm::vec3(0.1f * dt);
    }
    if (genesis::input::GetKeyState(genesis::input::Key::G_KEY) == genesis::input::InputState::PRESSED)
    {
        world.GetComponent<genesis::TransformComponent>(entity).mScale += glm::vec3(0.1f * dt);
    }
    if (genesis::input::GetKeyState(genesis::input::Key::E_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition.y += moveSpeed * dt;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::Q_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition.y -= moveSpeed * dt;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::A_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition += dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::GetKeyState(genesis::input::Key::D_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition -= dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::GetKeyState(genesis::input::Key::W_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition += dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::S_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition -= dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::UP_ARROW_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPitch += lookSpeed * dt;
        if (cameraComponent.mPitch >= 2 * genesis::math::PI)
        {
            cameraComponent.mPitch = cameraComponent.mPitch - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::DOWN_ARROW_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPitch -= lookSpeed * dt;
        if (cameraComponent.mPitch <= 0.0f)
        {
            cameraComponent.mPitch = 2 * genesis::math::PI + cameraComponent.mPitch;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::LEFT_ARROW_KEY)== genesis::input::InputState::PRESSED)
    {
        cameraComponent.mYaw += lookSpeed * dt;
        if (cameraComponent.mYaw >= 2 * genesis::math::PI)
        {
            cameraComponent.mYaw = cameraComponent.mYaw - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::GetKeyState(genesis::input::Key::RIGHT_ARROW_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mYaw -= lookSpeed * dt;
        if (cameraComponent.mYaw <= 0.0f)
        {
            cameraComponent.mYaw = 2 * genesis::math::PI + cameraComponent.mYaw;
        }
    }
    
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

    auto rayDirection = genesis::math::ComputeMouseRayDirection(mViewMatrix, mProjectionMatrix, windowComponent.mRenderableWidth, windowComponent.mRenderableHeight);

    auto t = 0.0f;
    genesis::math::RayToPlaneIntersection(cameraComponent.mPosition, rayDirection, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), t);

    auto pointOfContact = cameraComponent.mPosition + t * rayDirection;
    auto vecFromPlayerToMouse = pointOfContact - world.GetComponent<genesis::TransformComponent>(entity).mPosition;
    auto arctan = genesis::math::Arctan2(vecFromPlayerToMouse.x, vecFromPlayerToMouse.y);
    world.GetComponent<genesis::TransformComponent>(entity).mRotation.z = -arctan;
    
    if (glm::length(vecFromPlayerToMouse) > 0.001f )
    {
        world.GetComponent<genesis::TransformComponent>(entity).mPosition += glm::normalize(vecFromPlayerToMouse) * 0.002f * dt;
    }
    
    
    //Log(LogType::INFO, "Z: %.4f, Scale: %.4f", world.GetComponent<genesis::TransformComponent>(entity).mPosition.z, world.GetComponent<genesis::TransformComponent>(entity).mScale.z);
}

///------------------------------------------------------------------------------------------------

