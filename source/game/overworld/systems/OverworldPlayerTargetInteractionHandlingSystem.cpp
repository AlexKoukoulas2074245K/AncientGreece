///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetInteractionHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 10/04/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldPlayerTargetInteractionHandlingSystem.h"
#include "../components/OverworldInteractionComponent.h"
#include "../utils/OverworldInteractionUtils.h"
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
    static const std::string CITY_STATE_PREVIEW_VIEW_NAME = "city_state_preview";

    static const StringId UNIT_NAME_DS_KEY             = StringId("unit_name");
    static const StringId UNIT_MODEL_NAME_DS_KEY       = StringId("unit_model_name");
    static const StringId UNIT_HEALTH_DS_KEY           = StringId("unit_health");
    static const StringId UNIT_HEALTH_RED_DS_KEY       = StringId("unit_health_red");
    static const StringId UNIT_HEALTH_GREEN_DS_KEY     = StringId("unit_health_green");
    static const StringId UNIT_HEALTH_BLUE_DS_KEY      = StringId("unit_health_blue");
    static const StringId UNIT_PARTY_SIZE_DS_KEY       = StringId("unit_party_size");
    static const StringId UNIT_PARTY_SIZE_RED_DS_KEY   = StringId("unit_party_size_red");
    static const StringId UNIT_PARTY_SIZE_GREEN_DS_KEY = StringId("unit_party_size_green");
    static const StringId UNIT_PARTY_SIZE_BLUE_DS_KEY  = StringId("unit_party_size_blue");
    static const StringId UNIT_SCALE_X_DS_KEY          = StringId("unit_scale_x");
    static const StringId UNIT_SCALE_Y_DS_KEY          = StringId("unit_scale_y");
    static const StringId UNIT_SCALE_Z_DS_KEY          = StringId("unit_scale_z");
    static const StringId UNIT_PARTY_LINE_PREFIX       = StringId("unit_party_line_");

    static const StringId CITY_STATE_NAME_DS_KEY           = StringId("city_state_name");
    static const StringId CITY_STATE_RENOWN_DS_KEY         = StringId("city_state_renown");
    static const StringId CITY_STATE_RENOWN_RED_DS_KEY     = StringId("city_state_renown_red");
    static const StringId CITY_STATE_RENOWN_GREEN_DS_KEY   = StringId("city_state_renown_green");
    static const StringId CITY_STATE_RENOWN_BLUE_DS_KEY    = StringId("city_state_renown_blue");
    static const StringId CITY_STATE_GARISSON_DS_KEY       = StringId("city_state_garisson");
    static const StringId CITY_STATE_GARISSON_RED_DS_KEY   = StringId("city_state_garisson_red");
    static const StringId CITY_STATE_GARISSON_GREEN_DS_KEY = StringId("city_state_garisson_green");
    static const StringId CITY_STATE_GARISSON_BLUE_DS_KEY  = StringId("city_state_garisson_blue");
    static const StringId CITY_STATE_DESCRIPTION_DS_KEY    = StringId("city_state_description");

    static const int UNIT_INTERACTION_PARTY_LINES_COUNT = 7;
}

///-----------------------------------------------------------------------------------------------

OverworldPlayerTargetInteractionHandlingSystem::OverworldPlayerTargetInteractionHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldPlayerTargetInteractionHandlingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& overworldInteractionComponent = world.GetComponent<OverworldInteractionComponent>(entityId);
        
        if (world.HasEntity(overworldInteractionComponent.mInteraction.mInstigatorEntityId) && world.HasEntity(overworldInteractionComponent.mInteraction.mOtherEntityId))
        {
            SaveInteractionToHistory(overworldInteractionComponent.mInteraction.mInstigatorEntityId,  overworldInteractionComponent.mInteraction.mOtherEntityId, overworldInteractionComponent.mInteraction.mInstigatorEntityName, overworldInteractionComponent.mInteraction.mOtherEntityName);
            
            const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(overworldInteractionComponent.mInteraction.mOtherEntityId);
            
            // Unit Intraction
            if (world.HasComponent<UnitStatsComponent>(overworldInteractionComponent.mInteraction.mOtherEntityId))
            {
                const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(overworldInteractionComponent.mInteraction.mOtherEntityId);
                           
                const auto unitName           = unitStatsComponent.mStats.mUnitName;
                const auto unitModelName      = GetUnitModelName(unitStatsComponent.mStats.mUnitType);
                const auto unitHealth         = unitStatsComponent.mStats.mHealth;
                const auto unitHealthColor    = GetUnitHealthColor(unitStatsComponent);
                const auto unitPartySize      = GetUnitPartySize(unitStatsComponent);
                const auto unitPartySizeColor = GetUnitPartyColor(unitStatsComponent);
                const auto& unitPartyBuckets  = GetUnitPartyCountBuckets(unitStatsComponent);
                const auto& unitScale         = transformComponent.mScale;
                
                WriteValue(UNIT_NAME_DS_KEY, unitName.GetString());
                WriteValue(UNIT_MODEL_NAME_DS_KEY, unitModelName.GetString());
                WriteValue(UNIT_HEALTH_DS_KEY, std::to_string(unitHealth));
                WriteValue(UNIT_HEALTH_RED_DS_KEY, std::to_string(unitHealthColor.mRed));
                WriteValue(UNIT_HEALTH_GREEN_DS_KEY, std::to_string(unitHealthColor.mGreen));
                WriteValue(UNIT_HEALTH_BLUE_DS_KEY, std::to_string(unitHealthColor.mBlue));
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
                    WriteValue(StringId(UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), std::to_string(bucket.second) + " " + bucket.first.GetString() + (bucket.second > 1 ? "s" : ""));
                }
                while (lineCounter < UNIT_INTERACTION_PARTY_LINES_COUNT)
                {
                    WriteValue(StringId(UNIT_PARTY_LINE_PREFIX.GetString() + std::to_string(lineCounter++)), "");
                }
                
                view::QueueView(UNIT_INTERACTION_VIEW_NAME);
            }
            // City State interaction
            else
            {
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
                WriteValue(CITY_STATE_DESCRIPTION_DS_KEY, cityStateInfo.mDescription);
                view::QueueView(CITY_STATE_PREVIEW_VIEW_NAME);
            }
        }
        
        world.DestroyEntity(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

}
