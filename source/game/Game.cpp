///------------------------------------------------------------------------------------------------
///  Game.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "overworld/components/OverworldWaypointTargetComponent.h"
#include "overworld/systems/OverworldCameraControllerSystem.h"
#include "overworld/systems/OverworldMovementControllerSystem.h"
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
#include "../engine/rendering/systems/ModelAnimationSystem.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/components/ScriptComponent.h"
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
    
    world.AddSystem(std::make_unique<overworld::OverworldTargetSelectionSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldMovementControllerSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldCameraControllerSystem>());
    
    world.AddSystem(std::make_unique<genesis::rendering::ModelAnimationSystem>());
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

///------------------------------------------------------------------------------------------------

void Game::VOnGameInit()
{
    genesis::rendering::LoadStaticModelByName("map", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map"));
    genesis::rendering::LoadStaticModelByName("map_edge", glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_1"));
    genesis::rendering::LoadStaticModelByName("map_edge", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_2"));
    genesis::rendering::LoadStaticModelByName("map_edge", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_3"));
    genesis::rendering::LoadStaticModelByName("map_edge", glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_4"));
    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
    genesis::rendering::AddLightSource(glm::vec3(2.0f, 2.0f, 0.0f), 4.0f);
    
    genesis::rendering::LoadAnimatedModelByName("spartan", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.004f, 0.004f, 0.004f), StringId("player"));
    
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
}

///------------------------------------------------------------------------------------------------

