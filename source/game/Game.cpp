///------------------------------------------------------------------------------------------------
///  Game.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "components/UnitStatsComponent.h"
#include "overworld/components/HighlightableComponent.h"
#include "overworld/components/OverworldTargetComponent.h"
#include "overworld/systems/HighlightingSystem.h"
#include "overworld/systems/OverworldCameraControllerSystem.h"
#include "overworld/systems/OverworldLocationInteractionSystem.h"
#include "overworld/systems/OverworldMapPickingInfoSystem.h"
#include "overworld/systems/OverworldMovementControllerSystem.h"
#include "overworld/systems/OverworldPlayerTargetSelectionSystem.h"
#include "overworld/utils/OverworldCityStateUtils.h"
#include "utils/CityStateInfoUtils.h"
#include "utils/KeyValueUtils.h"
#include "view/components/ViewQueueSingletonComponent.h"
#include "view/systems/ViewManagementSystem.h"
#include "view/utils/ViewUtils.h"
#include "../engine/ECS.h"
#include "../engine/common/components/TransformComponent.h"
#include "../engine/common/utils/Logging.h"
#include "../engine/common/utils/MathUtils.h"
#include "../engine/debug/components/DebugViewStateSingletonComponent.h"
#include "../engine/debug/systems/ConsoleManagementSystem.h"
#include "../engine/debug/utils/ConsoleCommandUtils.h"
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

#include <thread>

///------------------------------------------------------------------------------------------------

static int SPARTAN_COUNT = 30;
static float dtAccum = 0.0f;
static float dtAccum2 = 0.0f;
#if !defined(NDEBUG)
extern float DEBUG_TEXTBOX_SIZE_DX;
extern float DEBUG_TEXTBOX_SIZE_DY;
extern float DEBUG_TEXTBOX_DX;
extern float DEBUG_TEXTBOX_DY;
#endif

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
    
    world.AddSystem(std::make_unique<overworld::OverworldMapPickingInfoSystem>());
    world.AddSystem(std::make_unique<overworld::HighlightingSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldLocationInteractionSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldPlayerTargetSelectionSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldMovementControllerSystem>());
    world.AddSystem(std::make_unique<overworld::OverworldCameraControllerSystem>());
    
    world.AddSystem(std::make_unique<genesis::rendering::ModelAnimationSystem>(), genesis::ecs::SystemOperationMode::MULTI_THREADED);
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

///------------------------------------------------------------------------------------------------

void Game::VOnGameInit()
{
    RegisterConsoleCommands();
    LoadGameFonts();
    LoadCityStateInfo();
    overworld::PopulateOverworldCityStates();
    LoadAndCreateOverworldMapComponents();
    
    auto& world = genesis::ecs::World::GetInstance();

    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
    genesis::rendering::AddLightSource(glm::vec3(2.0f, 2.0f, 0.0f), 4.0f);
    
    auto playerEntity = genesis::rendering::LoadAndCreateAnimatedModelByName("horseman", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0004f, 0.0004f, 0.0004f), StringId("player"));
    auto playerStatsComponent = std::make_unique<UnitStatsComponent>();
    playerStatsComponent->mSpeedMultiplier = 4.0f;
    world.AddComponent<UnitStatsComponent>(playerEntity, std::move(playerStatsComponent));
    world.AddComponent<overworld::HighlightableComponent>(playerEntity, std::make_unique<overworld::HighlightableComponent>());
    
    for (int i = 0; i < SPARTAN_COUNT; ++i)
    {
        auto spartanEntity = genesis::rendering::LoadAndCreateAnimatedModelByName((i % 2 == 0 ? "spartan2" : "horseman"), glm::vec3(genesis::math::RandomFloat(-0.2f, 0.2f), genesis::math::RandomFloat(-0.2f, 0.2f), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), (i % 2 == 0 ? glm::vec3(0.004f, 0.004f, 0.004f) : glm::vec3(0.0004f, 0.0004f, 0.0004f)), StringId("spartan_" + std::to_string(i)));
        world.AddComponent<overworld::HighlightableComponent>(spartanEntity, std::make_unique<overworld::HighlightableComponent>());
        world.AddComponent<UnitStatsComponent>(spartanEntity, std::make_unique<UnitStatsComponent>());
    }
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
            if (!world.HasComponent<overworld::OverworldTargetComponent>(entity))
            {
                const auto& position = world.GetComponent<genesis::TransformComponent>(entity);
                auto component = std::make_unique<overworld::OverworldTargetComponent>();
                component->mTargetPosition = glm::vec3(genesis::math::RandomFloat(position.mPosition.x - 0.2f, position.mPosition.x + 0.2f),genesis::math::RandomFloat(position.mPosition.y -0.2f, position.mPosition.y + 0.2f),0.0f);
                world.AddComponent<overworld::OverworldTargetComponent>(entity, std::move(component));
            }
        }
    }
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(dtAccum/2) * 2;
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(dtAccum/2) * 2;
    
//    if (genesis::input::GetButtonState(genesis::input::Button::RIGHT_BUTTON) == genesis::input::InputState::TAPPED)
//    {
//        if (world.FindEntityWithName(StringId("viewname")) != genesis::ecs::NULL_ENTITY_ID)
//        {
//            view::DestroyView(world.FindEntityWithName(StringId("viewname")));
//        }
//        else
//        {
//            view::QueueView("test", StringId("viewname"));
//        }
//    }
#if !defined(NDEBUG)
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
#endif
}

///------------------------------------------------------------------------------------------------

void Game::RegisterConsoleCommands() const
{
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    genesis::debug::RegisterConsoleCommand(StringId("free_cam"), [](const std::vector<std::string>& commandTextComponents)
    {
        static const std::unordered_set<std::string> sAllowedOptions = { "on", "off" };

        const std::string USAGE_STRING = "Usage: free_cam on|off";

        if (commandTextComponents.size() != 2 || sAllowedOptions.count(StringToLower(commandTextComponents[1])) == 0)
        {
            return genesis::debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        const auto& world = genesis::ecs::World::GetInstance();
        auto& debugViewStateComponent = world.GetSingletonComponent<genesis::debug::DebugViewStateSingletonComponent>();

        debugViewStateComponent.mFreeCamDebugEnabled = StringToLower(commandTextComponents[1]) == "on";

        return genesis::debug::ConsoleCommandResult(true);
    });
    
    genesis::debug::RegisterConsoleCommand(StringId("reset_cam"), [](const std::vector<std::string>& commandTextComponents)
    {
        const std::string USAGE_STRING = "Usage: reset_cam";

        if (commandTextComponents.size() != 1)
        {
            return genesis::debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        auto& world = genesis::ecs::World::GetInstance();
        world.RemoveSingletonComponent<genesis::rendering::CameraSingletonComponent>();
        world.SetSingletonComponent<genesis::rendering::CameraSingletonComponent>(std::make_unique<genesis::rendering::CameraSingletonComponent>());
        return genesis::debug::ConsoleCommandResult(true);
    });
#endif
}

///------------------------------------------------------------------------------------------------

void Game::LoadGameFonts() const
{
    genesis::rendering::LoadFont(StringId("ancient_greek_font"), 16, 16);
}

///------------------------------------------------------------------------------------------------

void Game::LoadAndCreateOverworldMapComponents() const
{
    genesis::rendering::LoadAndCreateStaticModelByName("map", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_1"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_2"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_3"));
    genesis::rendering::LoadAndCreateStaticModelByName("map_edge", glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_4"));
}

///------------------------------------------------------------------------------------------------
