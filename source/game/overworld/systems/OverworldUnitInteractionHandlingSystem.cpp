///------------------------------------------------------------------------------------------------
///  OverworldUnitInteractionHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldUnitInteractionHandlingSystem.h"
#include "../components/OverworldUnitInteractionComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/KeyValueUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../view/utils/ViewUtils.h"
#include "../../../engine/common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const std::string UNIT_INTERACTION_VIEW_NAME = "unit_interaction";

    static const StringId UNIT_NAME_DS_KEY             = StringId("unit_name");
    static const StringId UNIT_MODEL_NAME_DS_KEY       = StringId("unit_model_name");
    static const StringId UNIT_PARTY_SIZE_DS_KEY       = StringId("unit_party_size");
    static const StringId UNIT_PARTY_SIZE_RED_DS_KEY   = StringId("unit_party_size_red");
    static const StringId UNIT_PARTY_SIZE_GREEN_DS_KEY = StringId("unit_party_size_green");
    static const StringId UNIT_PARTY_SIZE_BLUE_DS_KEY  = StringId("unit_party_size_blue");
    static const StringId UNIT_SCALE_X_DS_KEY          = StringId("unit_scale_x");
    static const StringId UNIT_SCALE_Y_DS_KEY          = StringId("unit_scale_y");
    static const StringId UNIT_SCALE_Z_DS_KEY          = StringId("unit_scale_z");
}

///-----------------------------------------------------------------------------------------------

OverworldUnitInteractionHandlingSystem::OverworldUnitInteractionHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldUnitInteractionHandlingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& overworldUnitInteractionComponent = world.GetComponent<OverworldUnitInteractionComponent>(entityId);
        
        if (world.HasEntity(overworldUnitInteractionComponent.mInstigatorEntityId) && world.HasEntity(overworldUnitInteractionComponent.mOtherEntityId))
        {
            const auto& unitTransformComponent = world.GetComponent<genesis::TransformComponent>(overworldUnitInteractionComponent.mOtherEntityId);
            const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(overworldUnitInteractionComponent.mOtherEntityId);
            
            const auto unitName           = unitStatsComponent.mUnitName;
            const auto unitModelName      = unitStatsComponent.mModelName;
            const auto unitPartySize      = unitStatsComponent.mPartySize;
            const auto unitPartySizeColor = GetUnitPartyColor(unitStatsComponent);
            const auto& unitScale         = unitTransformComponent.mScale;
            
            WriteValue(UNIT_NAME_DS_KEY, unitName.GetString());
            WriteValue(UNIT_MODEL_NAME_DS_KEY, unitModelName.GetString());
            WriteValue(UNIT_PARTY_SIZE_DS_KEY, std::to_string(unitPartySize));
            WriteValue(UNIT_PARTY_SIZE_RED_DS_KEY, std::to_string(unitPartySizeColor.mRed));
            WriteValue(UNIT_PARTY_SIZE_GREEN_DS_KEY, std::to_string(unitPartySizeColor.mGreen));
            WriteValue(UNIT_PARTY_SIZE_BLUE_DS_KEY, std::to_string(unitPartySizeColor.mBlue));
            WriteValue(UNIT_SCALE_X_DS_KEY, std::to_string(unitScale.x));
            WriteValue(UNIT_SCALE_Y_DS_KEY, std::to_string(unitScale.y));
            WriteValue(UNIT_SCALE_Z_DS_KEY, std::to_string(unitScale.z));

            view::QueueView(UNIT_INTERACTION_VIEW_NAME);
        }
        
        world.DestroyEntity(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

}
