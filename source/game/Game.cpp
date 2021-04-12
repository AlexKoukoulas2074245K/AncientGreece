///------------------------------------------------------------------------------------------------
///  Game.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "GameContexts.h"
#include "battle/systems/BattleAttackTriggerHandlingSystem.h"
#include "battle/systems/BattleCameraControllerSystem.h"
#include "battle/systems/BattleCollisionHandlingSystem.h"
#include "battle/systems/BattleDamageApplicationSystem.h"
#include "battle/systems/BattleDestructionTimerProcessingSystem.h"
#include "battle/systems/BattleMovementControllerSystem.h"
#include "battle/systems/BattleTargetAcquisitionSystem.h"
#include "components/CollidableComponent.h"
#include "components/UnitStatsComponent.h"
#include "overworld/components/HighlightableComponent.h"
#include "overworld/components/OverworldTargetComponent.h"
#include "overworld/systems/HighlightingSystem.h"
#include "overworld/systems/OverworldBattleProcessingSystem.h"
#include "overworld/systems/OverworldCameraControllerSystem.h"
#include "overworld/systems/OverworldMapPickingInfoSystem.h"
#include "overworld/systems/OverworldMovementControllerSystem.h"
#include "overworld/systems/OverworldPlayerTargetInteractionHandlingSystem.h"
#include "overworld/systems/OverworldPlayerTargetSelectionSystem.h"
#include "overworld/utils/OverworldUtils.h"
#include "scene/systems/SceneUpdaterSystem.h"
#include "systems/ModelAnimationTogglingSystem.h"
#include "utils/CityStateInfoUtils.h"
#include "utils/UnitFactoryUtils.h"
#include "utils/UnitInfoUtils.h"
#include "view/components/ViewQueueSingletonComponent.h"
#include "view/systems/ViewManagementSystem.h"
#include "view/utils/ViewUtils.h"
#include "../engine/ECS.h"
#include "../engine/animation/systems/ModelAnimationSystem.h"
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
#include "../engine/rendering/utils/HeightMapUtils.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/components/ScriptComponent.h"
#include "../engine/scripting/systems/ScriptingSystem.h"

#include <thread>

///------------------------------------------------------------------------------------------------

static int SPARTAN_COUNT = 40;
static float dtAccum = 0.0f;
static float dtAccum2 = 0.0f;
#if !defined(NDEBUG)
extern float DEBUG_TEXTBOX_SIZE_DX;
extern float DEBUG_TEXTBOX_SIZE_DY;
extern float DEBUG_TEXTBOX_DX;
extern float DEBUG_TEXTBOX_DY;
extern float DEBUG_MODEL_RX;
extern float DEBUG_MODEL_RY;
extern float DEBUG_MODEL_RZ;
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
    
    world.AddSystem(std::make_unique<ModelAnimationTogglingSystem>());
    world.AddSystem(std::make_unique<view::ViewManagementSystem>());
    
    world.AddSystem(std::make_unique<battle::BattleCameraControllerSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleTargetAcquisitionSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleMovementControllerSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleCollisionHandlingSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleAttackTriggerHandlingSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleDamageApplicationSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleDestructionTimerProcessingSystem>(), BATTLE_CONTEXT);
    
    world.AddSystem(std::make_unique<overworld::OverworldMapPickingInfoSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::HighlightingSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldPlayerTargetSelectionSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldMovementControllerSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldPlayerTargetInteractionHandlingSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldBattleProcessingSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldCameraControllerSystem>(), MAP_CONTEXT);
    
    world.AddSystem(std::make_unique<scene::SceneUpdaterSystem>());
    world.AddSystem(std::make_unique<genesis::animation::ModelAnimationSystem>(), 0, genesis::ecs::SystemOperationMode::MULTI_THREADED);
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

///------------------------------------------------------------------------------------------------

void Game::VOnGameInit()
{
    auto& world = genesis::ecs::World::GetInstance();
    world.ChangeContext(MAP_CONTEXT);
    
    RegisterConsoleCommands();
    LoadGameFonts();
    LoadUnitBaseStats();
    LoadCityStateInfo();
    
    overworld::PopulateOverworldEntities();

    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 0.5f), 1.0f);
    
    const tsl::robin_map<int, StringId> intToModelType =
    {
        { 0, StringId("Spearman") },
        { 1, StringId("Elite Spearman") },
        { 2, StringId("Horse Archer") },
    };
    
    auto playerEntity = CreateUnit(StringId("Horse Archer"), StringId("ALEX"), StringId("player"));
    const auto partySize = genesis::math::RandomInt(0, 80);
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(playerEntity);

    for (auto j = 0; j < partySize; ++j)
    {
        const auto unitTypeRng = intToModelType.at(genesis::math::RandomInt(0, 2));
        unitStatsComponent.mParty.push_back(GetUnitBaseStats(unitTypeRng));
    }
    
    for (int i = 0; i < SPARTAN_COUNT; ++i)
    {
        auto position = glm::vec3(genesis::math::RandomFloat(-0.2f, 0.2f), genesis::math::RandomFloat(-0.2f, 0.2f), 0.0f);
        position.z = -overworld::GetTerrainHeightAtPosition(position);
        
        auto unitTypeName = intToModelType.at(i % 3);
        auto entityName = StringId("overworld_unit");
        
        auto unitEntity = CreateUnit(unitTypeName, GetRandomAvailableUnitName(), entityName, position);
        
        const auto partySize = genesis::math::RandomInt(0, 80);
        auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(unitEntity);
        
        for (auto j = 0; j < partySize; ++j)
        {
            const auto unitTypeRng = intToModelType.at(genesis::math::RandomInt(0, 2));
            unitStatsComponent.mParty.push_back(GetUnitBaseStats(unitTypeRng));
        }
    }
}

///------------------------------------------------------------------------------------------------

void Game::VOnUpdate(float& dt)
{
    auto& world = genesis::ecs::World::GetInstance();
    
    auto& lightStoreComponent = world.GetSingletonComponent<genesis::rendering::LightStoreSingletonComponent>();
    
    
    dtAccum2 += dt;
    
    const auto dayCycleSpeed = 1.0f/10.0f;
    dtAccum += dayCycleSpeed * dt;
    
    const auto currentTimeStamp = std::fmod(dtAccum, genesis::math::PI * 2.0f);
    Log(LogType::INFO, "%.6f", currentTimeStamp);
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(currentTimeStamp);
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(currentTimeStamp);
    
    const auto dayCycleFactor = (currentTimeStamp/(genesis::math::PI * 2.0f)) * 24.0f;
    
    std::string currentPeriod = "Night";
    
    if (dayCycleFactor <= 3.0f) currentPeriod = "Midnight";
    else if (dayCycleFactor <= 6.0f) currentPeriod = "Dawn";
    else if (dayCycleFactor <= 9.0f) currentPeriod = "Early Morning";
    else if (dayCycleFactor <= 12.0f) currentPeriod = "Morning";
    else if (dayCycleFactor <= 15.0f) currentPeriod = "Midday";
    else if (dayCycleFactor <= 18.0f) currentPeriod = "Afternoon";
    else if (dayCycleFactor <= 21.0f) currentPeriod = "Dusk";
    else if (dayCycleFactor <= 24.0f) currentPeriod = "Night";
    
    world.DestroyEntities(world.FindAllEntitiesWithName(StringId("time_period")));
    genesis::rendering::RenderText(currentPeriod, StringId("ancient_greek_font"), 0.1f, glm::vec3(-0.85f, -0.85f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), false, StringId("time_period"));
    //    Log(LogType::INFO, "%.6f, %.6f, %.6f", lightStoreComponent.mLightPositions[0].x, lightStoreComponent.mLightPositions[0].y, lightStoreComponent.mLightPositions[0].z);
#if !defined(NDEBUG)
//    if (genesis::input::GetButtonState(genesis::input::Button::RIGHT_BUTTON) == genesis::input::InputState::TAPPED)
//    {
//        if (world.FindEntityWithName(StringId("unit_interaction")) != genesis::ecs::NULL_ENTITY_ID)
//        {
//            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")));
//        }
//        else
//        {
//            view::QueueView("unit_interaction", StringId("unit_interaction"));
//        }
//    }
    if (world.GetContext() == BATTLE_CONTEXT && genesis::input::GetKeyState(genesis::input::Key::F_KEY) == genesis::input::InputState::TAPPED)
    {
        auto ents = world.FindAllEntitiesWithName(StringId("battle_unit"));
        auto randomIndex = genesis::math::RandomInt(0, ents.size() - 1);
        world.DestroyEntity(ents[randomIndex]);
    }
    
    if (world.FindEntityWithName(StringId("unit_interaction")) != genesis::ecs::NULL_ENTITY_ID)
    {
        if (genesis::input::GetKeyState(genesis::input::Key::LEFT_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_MODEL_RX -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::RIGHT_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_MODEL_RX += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::UP_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_MODEL_RY += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::DOWN_ARROW_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_MODEL_RY -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::T_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_MODEL_RZ += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::G_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_MODEL_RZ -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::F_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_SIZE_DX -= 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
        }
        if (genesis::input::GetKeyState(genesis::input::Key::H_KEY) == genesis::input::InputState::TAPPED)
        {
            DEBUG_TEXTBOX_SIZE_DX += 0.05f;
            view::DestroyView(world.FindEntityWithName(StringId("unit_interaction")), StringId());
            view::QueueView("unit_interaction", StringId("unit_interaction"));
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
    
    genesis::debug::RegisterConsoleCommand(StringId("scene_debug"), [](const std::vector<std::string>& commandTextComponents)
    {
        static const std::unordered_set<std::string> sAllowedOptions = { "on", "off" };

        const std::string USAGE_STRING = "Usage: free_cam on|off";

        if (commandTextComponents.size() != 2 || sAllowedOptions.count(StringToLower(commandTextComponents[1])) == 0)
        {
            return genesis::debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        const auto& world = genesis::ecs::World::GetInstance();
        auto& debugViewStateComponent = world.GetSingletonComponent<genesis::debug::DebugViewStateSingletonComponent>();

        debugViewStateComponent.mSceneGraphDisplayEnabled = StringToLower(commandTextComponents[1]) == "on";

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
