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
    
    static const StringId BATTLE_RESULT_MODEL_NAME_DS_KEY      = StringId("battle_result_model_name");
    static const StringId BATTLE_RESULT_TEXT_DS_KEY            = StringId("battle_result_text");
    static const StringId BATTLE_RESULT_TEXT_RED_DS_KEY        = StringId("battle_result_text_red");
    static const StringId BATTLE_RESULT_TEXT_GREEN_DS_KEY      = StringId("battle_result_text_green");
    static const StringId BATTLE_RESULT_TEXT_BLUE_DS_KEY       = StringId("battle_result_text_blue");
    static const StringId BATTLE_PLAYER_CASUALTIES_LINE_PREFIX = StringId("player_casualties_line_");
    static const StringId BATTLE_ENEMY_CASUALTIES_LINE_PREFIX  = StringId("enemy_casualties_line_");

    static const glm::vec3 BATTLE_VICTORY_COLOR = glm::vec3(0.0f, 0.7f, 0.0f);
    static const glm::vec3 BATTLE_DEFEAT_COLOR  = glm::vec3(0.7f, 0.0f, 0.0f);

    static const int BATTLE_CASUALTIES_LINE_COUNT = 7;

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
    auto& battleStateComponent = world.GetSingletonComponent<BattleStateSingletonComponent>();
    
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
                
                if (battleSideComponent.mBattleSideLeaderUnitName == lastInteraction.mInstigatorUnitName)
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
            PrepareAndShowResultsView();
        } break;
    }
}

///-----------------------------------------------------------------------------------------------

void BattleEndHandlingSystem::PrepareAndShowResultsView() const
{
    const auto& lastInteraction = overworld::GetLastInteraction();
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& battleStateComponent = world.GetSingletonComponent<BattleStateSingletonComponent>();
    
    auto playerVictorious = (lastInteraction.mInstigatorUnitName == overworld::GetPlayerUnitName() && battleStateComponent.mBattleResult == BattleResult::ATTACKER_VICTORIOUS) || (lastInteraction.mOtherUnitName == overworld::GetPlayerUnitName() && battleStateComponent.mBattleResult == BattleResult::DEFENDER_VICTORIOUS);
    
    if (playerVictorious)
    {
        WriteValue(BATTLE_RESULT_MODEL_NAME_DS_KEY, "battle_result_victory");
        WriteValue(BATTLE_RESULT_TEXT_DS_KEY, "Victory");
        WriteValue(BATTLE_RESULT_TEXT_RED_DS_KEY, std::to_string(BATTLE_VICTORY_COLOR.r));
        WriteValue(BATTLE_RESULT_TEXT_GREEN_DS_KEY, std::to_string(BATTLE_VICTORY_COLOR.g));
        WriteValue(BATTLE_RESULT_TEXT_BLUE_DS_KEY, std::to_string(BATTLE_VICTORY_COLOR.b));
    }
    else
    {
        WriteValue(BATTLE_RESULT_MODEL_NAME_DS_KEY, "battle_result_defeat");
        WriteValue(BATTLE_RESULT_TEXT_DS_KEY, "Defeat");
        WriteValue(BATTLE_RESULT_TEXT_RED_DS_KEY, std::to_string(BATTLE_DEFEAT_COLOR.r));
        WriteValue(BATTLE_RESULT_TEXT_GREEN_DS_KEY, std::to_string(BATTLE_DEFEAT_COLOR.g));
        WriteValue(BATTLE_RESULT_TEXT_BLUE_DS_KEY, std::to_string(BATTLE_DEFEAT_COLOR.b));
    }
    
    const auto otherLeaderName = lastInteraction.mInstigatorUnitName == overworld::GetPlayerUnitName() ? lastInteraction.mOtherUnitName : lastInteraction.mInstigatorUnitName;
    
    // Write player casualties
    auto lineCounter = 0;
    for (const auto& bucket: battleStateComponent.mLeaderNameToCasualtiesMap.at(overworld::GetPlayerUnitName()))
    {
        WriteValue(StringId(BATTLE_PLAYER_CASUALTIES_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), std::to_string(bucket.second) + " " + GetUnitCollectionString(bucket.first, bucket.second));
        
    }
    while (lineCounter < BATTLE_CASUALTIES_LINE_COUNT)
    {
        WriteValue(StringId(BATTLE_PLAYER_CASUALTIES_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), "");
    }
    
    // Write other leader casualties
    lineCounter = 0;
    for (const auto& bucket: battleStateComponent.mLeaderNameToCasualtiesMap.at(otherLeaderName))
    {
        WriteValue(StringId(BATTLE_ENEMY_CASUALTIES_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), std::to_string(bucket.second) + " " + GetUnitCollectionString(bucket.first, bucket.second));
    }
    while (lineCounter < BATTLE_CASUALTIES_LINE_COUNT)
    {
        WriteValue(StringId(BATTLE_ENEMY_CASUALTIES_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), "");
    }
    
    view::QueueView(BATTLE_RESULT_VIEW_NAME);
}

///-----------------------------------------------------------------------------------------------

}
