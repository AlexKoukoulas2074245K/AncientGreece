///------------------------------------------------------------------------------------------------
///  BattleEndHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 15/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleEndHandlingSystem.h"
#include "../components/BattleSideComponent.h"
#include "../utils/BattleUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../GameContexts.h"
#include "../../overworld/utils/OverworldInteractionUtils.h"
#include "../../overworld/utils/OverworldUtils.h"
#include "../../overworld/components/OverworldInteractionComponent.h"
#include "../../utils/KeyValueUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../view/utils/ViewUtils.h"
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/animation/utils/AnimationUtils.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const std::string BATTLE_RESULT_VIEW_NAME = "battle_result";

    static const StringId BATTLE_RESULT_MODEL_NAME_DS_KEY = StringId("battle_result_model_name");
    static const StringId BATTLE_RESULT_TEXT_DS_KEY       = StringId("battle_result_text");
    static const StringId BATTLE_RESULT_TEXT_RED_DS_KEY   = StringId("battle_result_text_red");
    static const StringId BATTLE_RESULT_TEXT_GREEN_DS_KEY = StringId("battle_result_text_green");
    static const StringId BATTLE_RESULT_TEXT_BLUE_DS_KEY  = StringId("battle_result_text_blue");
    static const StringId BATTLE_CASUALTIES_LINE_PREFIX   = StringId("battle_casualties_line_");

    static const float CELEBRATION_TIME = 5.0f;
}

///-----------------------------------------------------------------------------------------------

BattleEndHandlingSystem::BattleEndHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleEndHandlingSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    const auto& lastInteraction = overworld::GetLastInteraction();
    auto& world = genesis::ecs::World::GetInstance();
    auto& battleStateComponent = world.GetSingletonComponent<LiveBattleStateSingletonComponent>();
    
    switch (battleStateComponent.mBattleState)
    {
        case BattleState::ONGOING:
        {
            auto attackingSideUnitCount = 0;
            auto defendingSideUnitCount = 0;
            
            for (const auto& entityId: entitiesToProcess)
            {
                if (IsUnitDead(entityId))
                {
                    continue;
                }
                
                const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(entityId);
                
                if (battleSideComponent.mBattleSideLeaderName == lastInteraction.mInstigatorEntityName)
                {
                    attackingSideUnitCount++;
                }
                else
                {
                    defendingSideUnitCount++;
                }
            }
            
            if (attackingSideUnitCount == 0)
            {
                battleStateComponent.mBattleResult = BattleResult::DEFENDER_VICTORIOUS;
                battleStateComponent.mBattleState = BattleState::UNITS_CELEBRATING;
                battleStateComponent.mCelebrationTimer = CELEBRATION_TIME;
            }
            else if (defendingSideUnitCount == 0)
            {
                battleStateComponent.mBattleResult = BattleResult::ATTACKER_VICTORIOUS;
                battleStateComponent.mBattleState = BattleState::UNITS_CELEBRATING;
                battleStateComponent.mCelebrationTimer = CELEBRATION_TIME;
            }
            
        } break;
            
            
        case BattleState::UNITS_CELEBRATING:
        {
            for (const auto& entityId: entitiesToProcess)
            {
                if (!IsUnitDead(entityId))
                {
                    genesis::animation::ChangeAnimation(entityId, StringId("celebrating"));
                }
            }
            
            battleStateComponent.mCelebrationTimer -= dt;
            if (battleStateComponent.mCelebrationTimer < 0.0f)
            {
                battleStateComponent.mBattleState = BattleState::FINISHED;
            }
        } break;
            
        case BattleState::FINISHED:
        {
            DestroyBattleEntities();
            overworld::PopulateOverworldEntities();
            overworld::TryLoadOverworldStateFromFile();
            overworld::PrepareOverworldCamera();
            world.ChangeContext(MAP_CONTEXT);
            
            auto playerVictorious = (lastInteraction.mInstigatorEntityName == overworld::GetPlayerEntityName() && battleStateComponent.mBattleResult == BattleResult::ATTACKER_VICTORIOUS) || (lastInteraction.mOtherEntityName == overworld::GetPlayerEntityName() && battleStateComponent.mBattleResult == BattleResult::DEFENDER_VICTORIOUS);
            
            if (playerVictorious)
            {
                WriteValue(BATTLE_RESULT_MODEL_NAME_DS_KEY, "battle_result_victory");
                WriteValue(BATTLE_RESULT_TEXT_DS_KEY, "Victory");
                WriteValue(BATTLE_RESULT_TEXT_RED_DS_KEY, std::to_string(genesis::colors::GREEN.r));
                WriteValue(BATTLE_RESULT_TEXT_GREEN_DS_KEY, std::to_string(genesis::colors::GREEN.g));
                WriteValue(BATTLE_RESULT_TEXT_BLUE_DS_KEY, std::to_string(genesis::colors::GREEN.b));
            }
            else
            {
                WriteValue(BATTLE_RESULT_MODEL_NAME_DS_KEY, "battle_result_defeat");
                WriteValue(BATTLE_RESULT_TEXT_DS_KEY, "Defeat");
                WriteValue(BATTLE_RESULT_TEXT_RED_DS_KEY, std::to_string(genesis::colors::RED.r));
                WriteValue(BATTLE_RESULT_TEXT_GREEN_DS_KEY, std::to_string(genesis::colors::RED.g));
                WriteValue(BATTLE_RESULT_TEXT_BLUE_DS_KEY, std::to_string(genesis::colors::RED.b));
            }
            
            view::QueueView(BATTLE_RESULT_VIEW_NAME);
        } break;
    }
}

///-----------------------------------------------------------------------------------------------

}
