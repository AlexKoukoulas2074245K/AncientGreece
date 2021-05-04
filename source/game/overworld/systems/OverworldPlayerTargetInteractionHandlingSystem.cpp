///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetInteractionHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 10/04/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldPlayerTargetInteractionHandlingSystem.h"
#include "../components/OverworldBattleStateComponent.h"
#include "../components/OverworldInteractionComponent.h"
#include "../utils/OverworldInteractionUtils.h"
#include "../utils/OverworldUtils.h"
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/KeyValueUtils.h"
#include "../../utils/CityStateInfoUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../view/utils/ViewUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const std::string UNIT_INTERACTION_VIEW_NAME   = "unit_interaction";
    static const std::string PLAYER_ATTACKED_VIEW_NAME    = "player_attacked";
    static const std::string ONGOING_BATTLE_VIEW_NAME     = "ongoing_battle";
    static const std::string CITY_STATE_PREVIEW_VIEW_NAME = "city_state_preview";

    static const StringId UNIT_NAME_DS_KEY             = StringId("unit_name");
    static const StringId ATTACKING_UNIT_NAME_DS_KEY   = StringId("attacking_unit_name");
    static const StringId DEFENDING_UNIT_NAME_DS_KEY   = StringId("defending_unit_name");

    static const StringId UNIT_MODEL_NAME_DS_KEY       = StringId("unit_model_name");
    static const StringId UNIT_PARTY_SIZE_DS_KEY       = StringId("unit_party_size");
    static const StringId UNIT_PARTY_SIZE_RED_DS_KEY   = StringId("unit_party_size_red");
    static const StringId UNIT_PARTY_SIZE_GREEN_DS_KEY = StringId("unit_party_size_green");
    static const StringId UNIT_PARTY_SIZE_BLUE_DS_KEY  = StringId("unit_party_size_blue");
    static const StringId UNIT_SCALE_X_DS_KEY          = StringId("unit_scale_x");
    static const StringId UNIT_SCALE_Y_DS_KEY          = StringId("unit_scale_y");
    static const StringId UNIT_SCALE_Z_DS_KEY          = StringId("unit_scale_z");
    static const StringId UNIT_PARTY_LINE_PREFIX       = StringId("unit_party_line_");

    static const StringId ATTACKING_UNIT_MODEL_NAME_DS_KEY       = StringId("attacking_unit_model_name");
    static const StringId ATTACKING_UNIT_PARTY_SIZE_DS_KEY       = StringId("attacking_unit_party_size");
    static const StringId ATTACKING_UNIT_PARTY_SIZE_RED_DS_KEY   = StringId("attacking_unit_party_size_red");
    static const StringId ATTACKING_UNIT_PARTY_SIZE_GREEN_DS_KEY = StringId("attacking_unit_party_size_green");
    static const StringId ATTACKING_UNIT_PARTY_SIZE_BLUE_DS_KEY  = StringId("attacking_unit_party_size_blue");
    static const StringId ATTACKING_UNIT_SCALE_X_DS_KEY          = StringId("attacking_unit_scale_x");
    static const StringId ATTACKING_UNIT_SCALE_Y_DS_KEY          = StringId("attacking_unit_scale_y");
    static const StringId ATTACKING_UNIT_SCALE_Z_DS_KEY          = StringId("attacking_unit_scale_z");
    static const StringId ATTACKING_UNIT_PARTY_LINE_PREFIX       = StringId("attacking_unit_party_line_");

    static const StringId DEFENDING_UNIT_MODEL_NAME_DS_KEY       = StringId("defending_unit_model_name");
    static const StringId DEFENDING_UNIT_PARTY_SIZE_DS_KEY       = StringId("defending_unit_party_size");
    static const StringId DEFENDING_UNIT_PARTY_SIZE_RED_DS_KEY   = StringId("defending_unit_party_size_red");
    static const StringId DEFENDING_UNIT_PARTY_SIZE_GREEN_DS_KEY = StringId("defending_unit_party_size_green");
    static const StringId DEFENDING_UNIT_PARTY_SIZE_BLUE_DS_KEY  = StringId("defending_unit_party_size_blue");
    static const StringId DEFENDING_UNIT_SCALE_X_DS_KEY          = StringId("defending_unit_scale_x");
    static const StringId DEFENDING_UNIT_SCALE_Y_DS_KEY          = StringId("defending_unit_scale_y");
    static const StringId DEFENDING_UNIT_SCALE_Z_DS_KEY          = StringId("defending_unit_scale_z");
    static const StringId DEFENDING_UNIT_PARTY_LINE_PREFIX       = StringId("defending_unit_party_line_");

    static const StringId CITY_STATE_NAME_DS_KEY           = StringId("city_state_name");
    static const StringId CITY_STATE_RENOWN_DS_KEY         = StringId("city_state_renown");
    static const StringId CITY_STATE_RENOWN_RED_DS_KEY     = StringId("city_state_renown_red");
    static const StringId CITY_STATE_RENOWN_GREEN_DS_KEY   = StringId("city_state_renown_green");
    static const StringId CITY_STATE_RENOWN_BLUE_DS_KEY    = StringId("city_state_renown_blue");
    static const StringId CITY_STATE_GARISSON_DS_KEY       = StringId("city_state_garisson");
    static const StringId CITY_STATE_GARISSON_RED_DS_KEY   = StringId("city_state_garisson_red");
    static const StringId CITY_STATE_GARISSON_GREEN_DS_KEY = StringId("city_state_garisson_green");
    static const StringId CITY_STATE_GARISSON_BLUE_DS_KEY  = StringId("city_state_garisson_blue");
    static const StringId CITY_STATE_RULER_NAME_DS_KEY     = StringId("city_state_ruler");
    static const StringId CITY_STATE_DESCRIPTION_DS_KEY    = StringId("city_state_description");

    static const int UNIT_INTERACTION_PARTY_LINES_COUNT = 10;
}

///-----------------------------------------------------------------------------------------------

OverworldPlayerTargetInteractionHandlingSystem::OverworldPlayerTargetInteractionHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetInteractionHandlingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto entitiesToProcess = entities;
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& overworldInteractionComponent = world.GetComponent<OverworldInteractionComponent>(entityId);
        
        if (world.HasEntity(overworldInteractionComponent.mInteraction.mInstigatorEntityId) && world.HasEntity(overworldInteractionComponent.mInteraction.mOtherEntityId))
        {
            SaveInteractionToHistory(overworldInteractionComponent.mInteraction.mInstigatorEntityId,  overworldInteractionComponent.mInteraction.mOtherEntityId, overworldInteractionComponent.mInteraction.mInstigatorUnitName, overworldInteractionComponent.mInteraction.mOtherUnitName);
            
            // Unit Intraction
            if ((overworldInteractionComponent.mInteraction.mInstigatorEntityId == GetPlayerEntity() || overworldInteractionComponent.mInteraction.mOtherEntityId == GetPlayerEntity()) && world.HasComponent<UnitStatsComponent>(overworldInteractionComponent.mInteraction.mOtherEntityId))
            {
                const auto& otherUnitStatsComponent = world.GetComponent<UnitStatsComponent>(overworldInteractionComponent.mInteraction.mOtherEntityId);
                const auto battleStateEntity = FindBattleStateEntityThatInvolvesUnit(otherUnitStatsComponent.mStats.mUnitName);
                
                // Either unit player interacted with is not in battle, or player is being attacked
                if (battleStateEntity == genesis::ecs::NULL_ENTITY_ID)
                {
                    ShowUnitInteractionView(overworldInteractionComponent);
                }
                // Unit player interacted with is in an ongoing battle
                else
                {
                    ShowOngoingBattleUnitIntractionView(battleStateEntity);
                }
            }
            // City State interaction
            else if (overworldInteractionComponent.mInteraction.mInstigatorEntityId == GetPlayerEntity())
            {
                ShowCityStateInteractionView(overworldInteractionComponent);
            }
        }
        
        world.DestroyEntity(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetInteractionHandlingSystem::ShowUnitInteractionView(const OverworldInteractionComponent& overworldInteractionComponent) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto isPlayerOtherEntity = overworldInteractionComponent.mInteraction.mOtherEntityId == GetPlayerEntity();
    
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(isPlayerOtherEntity ? overworldInteractionComponent.mInteraction.mInstigatorEntityId : overworldInteractionComponent.mInteraction.mOtherEntityId);
    
    const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(isPlayerOtherEntity ? overworldInteractionComponent.mInteraction.mInstigatorEntityId : overworldInteractionComponent.mInteraction.mOtherEntityId);
    
    const auto unitName           = unitStatsComponent.mStats.mUnitName;
    const auto unitModelName      = GetUnitModelName(unitStatsComponent.mStats.mUnitType);
    const auto unitPartySize      = GetUnitPartySize(unitStatsComponent);
    const auto unitPartySizeColor = GetUnitPartyColor(unitStatsComponent);
    const auto& unitPartyBuckets  = GetUnitPartyCountBuckets(unitStatsComponent, true);
    const auto& unitScale         = transformComponent.mScale;
    
    WriteValue(UNIT_NAME_DS_KEY, unitName.GetString());
    WriteValue(UNIT_MODEL_NAME_DS_KEY, unitModelName.GetString());
    WriteValue(UNIT_PARTY_SIZE_DS_KEY, std::to_string(unitPartySize));
    WriteValue(UNIT_PARTY_SIZE_RED_DS_KEY, std::to_string(unitPartySizeColor.mRed));
    WriteValue(UNIT_PARTY_SIZE_GREEN_DS_KEY, std::to_string(unitPartySizeColor.mGreen));
    WriteValue(UNIT_PARTY_SIZE_BLUE_DS_KEY, std::to_string(unitPartySizeColor.mBlue));
    WriteValue(UNIT_SCALE_X_DS_KEY, std::to_string(unitScale.x));
    WriteValue(UNIT_SCALE_Y_DS_KEY, std::to_string(unitScale.y));
    WriteValue(UNIT_SCALE_Z_DS_KEY, std::to_string(unitScale.z));
    
    auto lineCounter = 0;
    for (const auto& bucket: unitPartyBuckets)
    {
        WriteValue(StringId(UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), std::to_string(bucket.second) + " " + GetUnitCollectionString(bucket.first, bucket.second));
    }
    while (lineCounter < UNIT_INTERACTION_PARTY_LINES_COUNT)
    {
        WriteValue(StringId(UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), "");
    }
    
    view::QueueView(isPlayerOtherEntity ? PLAYER_ATTACKED_VIEW_NAME : UNIT_INTERACTION_VIEW_NAME);
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetInteractionHandlingSystem::ShowOngoingBattleUnitIntractionView(const genesis::ecs::EntityId battleStateEntity) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& ongoingBattleStateComponent = world.GetComponent<OverworldBattleStateComponent>(battleStateEntity);
    
    const auto attackingEntityId = GetOverworldUnitEntityByName(ongoingBattleStateComponent.mAttackingUnitName);
    const auto defendingEntityId = GetOverworldUnitEntityByName(ongoingBattleStateComponent.mDefendingUnitName);
    
    const auto& attackingUnitStatsComponent = world.GetComponent<UnitStatsComponent>(attackingEntityId);
    const auto& attackingUnitTransformComponent = world.GetComponent<genesis::TransformComponent>(attackingEntityId);
    
    const auto& defendingUnitStatsComponent = world.GetComponent<UnitStatsComponent>(defendingEntityId);
    const auto& defendingUnitTransformComponent = world.GetComponent<genesis::TransformComponent>(defendingEntityId);
    
    const auto attackingUnitName           = attackingUnitStatsComponent.mStats.mUnitName;
    const auto attackingUnitModelName      = GetUnitModelName(attackingUnitStatsComponent.mStats.mUnitType);
    const auto attackingUnitPartySize      = GetUnitPartySize(attackingUnitStatsComponent);
    const auto attackingUnitPartySizeColor = GetUnitPartyColor(attackingUnitStatsComponent);
    const auto& attackingUnitPartyBuckets  = GetUnitPartyCountBuckets(attackingUnitStatsComponent, true);
    const auto& attackingUnitScale         = attackingUnitTransformComponent.mScale;
    
    const auto defendingUnitName           = defendingUnitStatsComponent.mStats.mUnitName;
    const auto defendingUnitModelName      = GetUnitModelName(defendingUnitStatsComponent.mStats.mUnitType);
    const auto defendingUnitPartySize      = GetUnitPartySize(defendingUnitStatsComponent);
    const auto defendingUnitPartySizeColor = GetUnitPartyColor(defendingUnitStatsComponent);
    const auto& defendingUnitPartyBuckets  = GetUnitPartyCountBuckets(defendingUnitStatsComponent, true);
    const auto& defendingUnitScale         = defendingUnitTransformComponent.mScale;
    
    WriteValue(ATTACKING_UNIT_NAME_DS_KEY, attackingUnitName.GetString());
    WriteValue(DEFENDING_UNIT_NAME_DS_KEY, defendingUnitName.GetString());
    
    WriteValue(ATTACKING_UNIT_MODEL_NAME_DS_KEY, attackingUnitModelName.GetString());
    WriteValue(ATTACKING_UNIT_PARTY_SIZE_DS_KEY, std::to_string(attackingUnitPartySize));
    WriteValue(ATTACKING_UNIT_PARTY_SIZE_RED_DS_KEY, std::to_string(attackingUnitPartySizeColor.mRed));
    WriteValue(ATTACKING_UNIT_PARTY_SIZE_GREEN_DS_KEY, std::to_string(attackingUnitPartySizeColor.mGreen));
    WriteValue(ATTACKING_UNIT_PARTY_SIZE_BLUE_DS_KEY, std::to_string(attackingUnitPartySizeColor.mBlue));
    WriteValue(ATTACKING_UNIT_SCALE_X_DS_KEY, std::to_string(attackingUnitScale.x));
    WriteValue(ATTACKING_UNIT_SCALE_Y_DS_KEY, std::to_string(attackingUnitScale.y));
    WriteValue(ATTACKING_UNIT_SCALE_Z_DS_KEY, std::to_string(attackingUnitScale.z));
    
    WriteValue(DEFENDING_UNIT_MODEL_NAME_DS_KEY, defendingUnitModelName.GetString());
    WriteValue(DEFENDING_UNIT_PARTY_SIZE_DS_KEY, std::to_string(defendingUnitPartySize));
    WriteValue(DEFENDING_UNIT_PARTY_SIZE_RED_DS_KEY, std::to_string(defendingUnitPartySizeColor.mRed));
    WriteValue(DEFENDING_UNIT_PARTY_SIZE_GREEN_DS_KEY, std::to_string(defendingUnitPartySizeColor.mGreen));
    WriteValue(DEFENDING_UNIT_PARTY_SIZE_BLUE_DS_KEY, std::to_string(defendingUnitPartySizeColor.mBlue));
    WriteValue(DEFENDING_UNIT_SCALE_X_DS_KEY, std::to_string(defendingUnitScale.x));
    WriteValue(DEFENDING_UNIT_SCALE_Y_DS_KEY, std::to_string(defendingUnitScale.y));
    WriteValue(DEFENDING_UNIT_SCALE_Z_DS_KEY, std::to_string(defendingUnitScale.z));
    
    auto lineCounter = 0;
    for (const auto& bucket: attackingUnitPartyBuckets)
    {
        WriteValue(StringId(ATTACKING_UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), std::to_string(bucket.second) + " " + GetUnitCollectionString(bucket.first, bucket.second));
    }
    while (lineCounter < UNIT_INTERACTION_PARTY_LINES_COUNT)
    {
        WriteValue(StringId(ATTACKING_UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), "");
    }
    
    lineCounter = 0;
    for (const auto& bucket: defendingUnitPartyBuckets)
    {
        WriteValue(StringId(DEFENDING_UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), std::to_string(bucket.second) + " " + GetUnitCollectionString(bucket.first, bucket.second));
    }
    while (lineCounter < UNIT_INTERACTION_PARTY_LINES_COUNT)
    {
        WriteValue(StringId(DEFENDING_UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), "");
    }
    
    view::QueueView(ONGOING_BATTLE_VIEW_NAME);
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetInteractionHandlingSystem::ShowCityStateInteractionView(const OverworldInteractionComponent& overworldInteractionComponent) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto cityName = world.GetComponent<genesis::NameComponent>(overworldInteractionComponent.mInteraction.mOtherEntityId).mName;
    const auto& cityStateInfo = GetCityStateInfo(cityName);
    const auto garissonColor = GetCityStateGarissonColor(cityName);
    const auto renownColor = GetCityStateRenownColor(cityName);
    
    WriteValue(CITY_STATE_NAME_DS_KEY, cityName.GetString());
    WriteValue(CITY_STATE_RENOWN_DS_KEY, std::to_string(cityStateInfo.mRenown));
    WriteValue(CITY_STATE_RENOWN_RED_DS_KEY, std::to_string(renownColor.mRed));
    WriteValue(CITY_STATE_RENOWN_GREEN_DS_KEY, std::to_string(renownColor.mGreen));
    WriteValue(CITY_STATE_RENOWN_BLUE_DS_KEY, std::to_string(renownColor.mBlue));
    WriteValue(CITY_STATE_GARISSON_DS_KEY, std::to_string(cityStateInfo.mGarisson));
    WriteValue(CITY_STATE_GARISSON_RED_DS_KEY, std::to_string(garissonColor.mRed));
    WriteValue(CITY_STATE_GARISSON_GREEN_DS_KEY, std::to_string(garissonColor.mGreen));
    WriteValue(CITY_STATE_GARISSON_BLUE_DS_KEY, std::to_string(garissonColor.mBlue));
    WriteValue(CITY_STATE_RULER_NAME_DS_KEY, cityStateInfo.mRuler.GetString());
    WriteValue(CITY_STATE_DESCRIPTION_DS_KEY, cityStateInfo.mDescription);
    view::QueueView(CITY_STATE_PREVIEW_VIEW_NAME);
}

///-----------------------------------------------------------------------------------------------

}
