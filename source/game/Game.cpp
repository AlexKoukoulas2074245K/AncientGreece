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
#include "battle/systems/BattleEndHandlingSystem.h"
#include "battle/systems/BattleMovementControllerSystem.h"
#include "battle/systems/BattleTargetAcquisitionSystem.h"
#include "components/CollidableComponent.h"
#include "components/UnitStatsComponent.h"
#include "overworld/components/OverworldTargetComponent.h"
#include "overworld/systems/OverworldBattleProcessingSystem.h"
#include "overworld/systems/OverworldCameraControllerSystem.h"
#include "overworld/systems/OverworldDayTimeUpdaterSystem.h"
#include "overworld/systems/OverworldHighlightingSystem.h"
#include "overworld/systems/OverworldMapPickingInfoSystem.h"
#include "overworld/systems/OverworldMovementControllerSystem.h"
#include "overworld/systems/OverworldPlayerTargetInteractionHandlingSystem.h"
#include "overworld/systems/OverworldPlayerTargetSelectionSystem.h"
#include "overworld/utils/OverworldUtils.h"
#include "scene/systems/SceneUpdaterSystem.h"
#include "systems/ModelAnimationTogglingSystem.h"
#include "utils/CityStateInfoUtils.h"
#include "utils/KeyValueUtils.h"
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
#include "../engine/rendering/utils/HeightMapUtils.h"
#include "../engine/rendering/utils/MeshUtils.h"
#include "../engine/rendering/utils/ParticleUtils.h"
#include "../engine/rendering/systems/ParticleUpdaterSystem.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/components/ScriptComponent.h"
#include "../engine/scripting/systems/ScriptingSystem.h"

#include <thread>

///------------------------------------------------------------------------------------------------

static int SPARTAN_COUNT = 20;
//static float dtAccum2 = 0.0f;
#if !defined(NDEBUG)
extern float DEBUG_TEXTBOX_SIZE_DX;
extern float DEBUG_TEXTBOX_SIZE_DY;
extern float DEBUG_TEXTBOX_DX;
extern float DEBUG_TEXTBOX_DY;
extern float DEBUG_MODEL_X;
extern float DEBUG_MODEL_Y;
extern float DEBUG_MODEL_Z;
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
    
    world.AddSystem(std::make_unique<battle::BattleEndHandlingSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleCameraControllerSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleTargetAcquisitionSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleMovementControllerSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleCollisionHandlingSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleAttackTriggerHandlingSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleDamageApplicationSystem>(), BATTLE_CONTEXT);
    world.AddSystem(std::make_unique<battle::BattleDestructionTimerProcessingSystem>(), BATTLE_CONTEXT);
    
    world.AddSystem(std::make_unique<overworld::OverworldHighlightingSystem>(), 0);
    
    world.AddSystem(std::make_unique<overworld::OverworldDayTimeUpdaterSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldMapPickingInfoSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldPlayerTargetSelectionSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldMovementControllerSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldPlayerTargetInteractionHandlingSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldBattleProcessingSystem>(), MAP_CONTEXT);
    world.AddSystem(std::make_unique<overworld::OverworldCameraControllerSystem>(), MAP_CONTEXT);
    
    world.AddSystem(std::make_unique<scene::SceneUpdaterSystem>());
    world.AddSystem(std::make_unique<genesis::animation::ModelAnimationSystem>(), 0, genesis::ecs::SystemOperationMode::MULTI_THREADED);
    world.AddSystem(std::make_unique<genesis::rendering::ParticleUpdaterSystem>());
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
    
    if (!overworld::TryLoadOverworldStateFromFile())
    {
        const tsl::robin_map<int, StringId> intToModelType =
        {
            { 0, StringId("Spearman") },
            { 1, StringId("Elite Spearman") },
            { 2, StringId("Horse Archer") },
            { 3, StringId("Novice Spearman")},
            { 4, StringId("Novice Horse Archer")},
            { 5, StringId("Horse Lancer")},
            { 6, StringId("Barbarian Axeman")},
            { 7, StringId("Bandit Horseman")},
        };
        
        const auto mapEntity = overworld::GetMapEntity();
        auto position = glm::vec3(0.2f, 0.2f, 0.0f);
        position.z = genesis::rendering::GetTerrainHeightAtPosition(mapEntity, position);
        auto playerEntity = CreateUnit(StringId("Horse Archer"), StringId("ALEX"), overworld::GetPlayerEntityName(), position);
        
        const auto partySize = genesis::math::RandomInt(0, 80);
        auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(playerEntity);

        for (auto j = 0; j < partySize; ++j)
        {
            const auto unitTypeRng = intToModelType.at(genesis::math::RandomInt(0, intToModelType.size() - 1));
            unitStatsComponent.mParty.push_back(GetUnitBaseStats(unitTypeRng));
        }
        
        for (int i = 0; i < SPARTAN_COUNT; ++i)
        {
            auto position = glm::vec3(genesis::math::RandomFloat(-0.2f, 0.2f), genesis::math::RandomFloat(-0.2f, 0.2f), 0.0f);
            position.z = genesis::rendering::GetTerrainHeightAtPosition(mapEntity, position);
            
            auto unitTypeName = intToModelType.at(i % intToModelType.size());
            
            auto unitEntity = CreateUnit(unitTypeName, GetRandomAvailableUnitName(), overworld::GetGenericOverworldUnitEntityName(), position);
            
            const auto partySize = genesis::math::RandomInt(0, 80);
            auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(unitEntity);
            
            for (auto j = 0; j < partySize; ++j)
            {
                const auto unitTypeRng = intToModelType.at(genesis::math::RandomInt(0, intToModelType.size() - 1));
                unitStatsComponent.mParty.push_back(GetUnitBaseStats(unitTypeRng));
            }
        }
    }
    
    overworld::PrepareOverworldCamera();
}

///------------------------------------------------------------------------------------------------

void Game::VOnUpdate(float& dt)
{
    //auto& world = genesis::ecs::World::GetInstance();
    
#if !defined(NDEBUG)
    if (genesis::input::GetKeyState(genesis::input::Key::SPACEBAR_KEY) == genesis::input::InputState::PRESSED)
    {
        dt /= 10.0f;
    }
#endif
    
    //    Log(LogType::INFO, "%.6f, %.6f, %.6f", lightStoreComponent.mLightPositions[0].x, lightStoreComponent.mLightPositions[0].y, lightStoreComponent.mLightPositions[0].z);
#if !defined(NDEBUG)
//    const auto debugViewName = std::string("battle_result");
//    const auto debugViewNameId = StringId(debugViewName);
//    static const StringId BATTLE_RESULT_MODEL_NAME_DS_KEY = StringId("battle_result_model_name");
//    static const StringId BATTLE_RESULT_TEXT_DS_KEY       = StringId("battle_result_text");
//    static const StringId BATTLE_RESULT_TEXT_RED_DS_KEY   = StringId("battle_result_text_red");
//    static const StringId BATTLE_RESULT_TEXT_GREEN_DS_KEY = StringId("battle_result_text_green");
//    static const StringId BATTLE_RESULT_TEXT_BLUE_DS_KEY  = StringId("battle_result_text_blue");
//    
//    WriteValue(BATTLE_RESULT_MODEL_NAME_DS_KEY, "battle_result_victory");
//    WriteValue(BATTLE_RESULT_TEXT_DS_KEY, "Victory");
//    WriteValue(BATTLE_RESULT_TEXT_RED_DS_KEY, std::to_string(0.0f));
//    WriteValue(BATTLE_RESULT_TEXT_GREEN_DS_KEY, std::to_string(0.7f));
//    WriteValue(BATTLE_RESULT_TEXT_BLUE_DS_KEY, std::to_string(0.0f));
//    
//    if (genesis::input::GetButtonState(genesis::input::Button::RIGHT_BUTTON) == genesis::input::InputState::TAPPED)
//    {
//        if (world.FindEntityWithName(debugViewNameId) != genesis::ecs::NULL_ENTITY_ID)
//        {
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//        }
//        else
//        {
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//    }
//    
//    if (world.FindEntityWithName(debugViewNameId) != genesis::ecs::NULL_ENTITY_ID)
//    {
//        if (genesis::input::GetKeyState(genesis::input::Key::LEFT_ARROW_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_MODEL_X -= 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::RIGHT_ARROW_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_MODEL_X += 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::UP_ARROW_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_MODEL_Y += 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::DOWN_ARROW_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_MODEL_Y -= 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::T_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_MODEL_Z += 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::G_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_MODEL_Z -= 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::F_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_TEXTBOX_SIZE_DX -= 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, debugViewNameId);
//        }
//        if (genesis::input::GetKeyState(genesis::input::Key::H_KEY) == genesis::input::InputState::TAPPED)
//        {
//            DEBUG_TEXTBOX_SIZE_DX += 0.05f;
//            view::DestroyView(world.FindEntityWithName(debugViewNameId), StringId("close"));
//            view::QueueView(debugViewName, StringId(debugViewNameId));
//        }
//    }
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
