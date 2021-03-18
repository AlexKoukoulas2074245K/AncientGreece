///------------------------------------------------------------------------------------------------
///  Game.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "overworld/components/OverworldWaypointTargetComponent.h"
#include "overworld/systems/OverworldCameraControllerSystem.h"
#include "overworld/systems/OverworldMovementControllerSystem.h"
#include "overworld/systems/OverworldPlayerTargetSelectionSystem.h"
#include "view/components/ViewQueueSingletonComponent.h"
#include "view/systems/ViewManagementSystem.h"
#include "view/utils/ViewUtils.h"
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
#include "../engine/rendering/utils/FontUtils.h"
#include "../engine/rendering/utils/LightUtils.h"
#include "../engine/rendering/utils/MeshUtils.h"
#include "../engine/rendering/systems/ModelAnimationSystem.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/components/ScriptComponent.h"
#include "../engine/scripting/systems/ScriptingSystem.h"

///------------------------------------------------------------------------------------------------

static int SPARTAN_COUNT = 100;
static float dtAccum = 0.0f;
static float dtAccum2 = 0.0f;
extern float DEBUG_TEXTBOX_SIZE_DX;
extern float DEBUG_TEXTBOX_SIZE_DY;
extern float DEBUG_TEXTBOX_DX;
extern float DEBUG_TEXTBOX_DY;

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
    
    world.AddSystem(std::make_unique<view::ViewManagementSystem>());
    
    world.AddSystem(std::make_unique<overworld::OverworldPlayerTargetSelectionSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldMovementControllerSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldCameraControllerSystem>());
    
    world.AddSystem(std::make_unique<genesis::rendering::ModelAnimationSystem>());
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

///------------------------------------------------------------------------------------------------

void Game::VOnGameInit()
{
    genesis::rendering::LoadFont(StringId("ancient_greek_font"), 16, 16);
    genesis::rendering::LoadAndCreateStaticModelByName("map", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_1"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_2"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_3"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_4"));
    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
    genesis::rendering::AddLightSource(glm::vec3(2.0f, 2.0f, 0.0f), 4.0f);
    
    genesis::rendering::LoadAndCreateAnimatedModelByName("spartan", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.004f, 0.004f, 0.004f), StringId("player"));
    
    for (int i = 0; i < SPARTAN_COUNT; ++i)
    {
        genesis::rendering::LoadAndCreateAnimatedModelByName("spartan", glm::vec3(genesis::math::RandomFloat(-0.2f, 0.2f), genesis::math::RandomFloat(-0.2f, 0.2f), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.004f, 0.004f, 0.004f), StringId("spartan_" + std::to_string(i)));
    }
    
//    auto entity = genesis::rendering::LoadAndCreateGuiSprite("gui_base", "parchment", StringId("default_gui"));
//    auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entity);
//    transformComponent.mScale /= 2.0f;
//    
//    genesis::rendering::RenderText("This is a message to the owner 123", StringId("ancient_greek_font"), 0.08f, glm::vec3(-0.2f, 0.0f, -0.1f));
}

///------------------------------------------------------------------------------------------------

void Game::VOnUpdate(float& dt)
{
    auto& world = genesis::ecs::World::GetInstance();
    
    auto& lightStoreComponent = world.GetSingletonComponent<genesis::rendering::LightStoreSingletonComponent>();
    dt = world.GetSingletonComponent<view::ViewQueueSingletonComponent>().mActiveViewExists ? 0.0f : dt;
    
    dtAccum += dt;
    dtAccum2 += dt;
    
    if (dtAccum2 > 1.0f)
    {
        dtAccum2 = 0.0f;
        for (int i = 0; i < SPARTAN_COUNT; ++i)
        {
            auto entity = world.FindEntityWithName(StringId("spartan_" + std::to_string(i)));
            if (!world.HasComponent<overworld::OverworldWaypointTargetComponent>(entity))
            {
                const auto& position = world.GetComponent<genesis::TransformComponent>(entity);
                auto component = std::make_unique<overworld::OverworldWaypointTargetComponent>();
                component->mTargetPosition = glm::vec3(genesis::math::RandomFloat(position.mPosition.x - 0.2f, position.mPosition.x + 0.2f),genesis::math::RandomFloat(position.mPosition.y -0.2f, position.mPosition.y + 0.2f),0.0f);
                world.AddComponent<overworld::OverworldWaypointTargetComponent>(entity, std::move(component));
            }
        }
    }
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(dtAccum/2) * 2;
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(dtAccum/2) * 2;
    
    if (genesis::input::GetButtonState(genesis::input::Button::RIGHT_BUTTON) == genesis::input::InputState::TAPPED)
    {
        if (world.FindEntityWithName(StringId("viewname")) != genesis::ecs::NULL_ENTITY_ID)
        {
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
        }
        else
        {
            view::QueueView("test", StringId("viewname"));
        }
    }
    if (world.FindEntityWithName(StringId("viewname")) != genesis::ecs::NULL_ENTITY_ID)
    {
        if (genesis::input::GetKeyState(genesis::input::Key::LEFT_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_DX -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::RIGHT_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_DX += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::UP_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_DY += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::DOWN_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_DY -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::T_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_SIZE_DY += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::G_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_SIZE_DY -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::F_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_SIZE_DX -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::H_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_SIZE_DX += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
            view::QueueView("test", StringId("viewname"));
        }
    }
}

///------------------------------------------------------------------------------------------------

