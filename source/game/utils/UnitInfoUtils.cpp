///------------------------------------------------------------------------------------------------
///  UnitInfoUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/03/2021.
///------------------------------------------------------------------------------------------------

#include "UnitInfoUtils.h"
#include "../components/UnitAvailableNamesSingletonComponent.h"
#include "../components/UnitBaseStatsSingletonComponent.h"
#include "../components/UnitStatsComponent.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/resources/DataFileResource.h"
#include "../../engine/resources/ResourceLoadingService.h"

#include <json.hpp>

///------------------------------------------------------------------------------------------------

namespace
{
    static const std::string NAMES_LIST_FILE_PATH = genesis::resources::ResourceLoadingService::RES_DATA_ROOT + "names_list.dat";
    static const std::string UNIT_BASE_STATS_FILE_PATH = genesis::resources::ResourceLoadingService::RES_DATA_ROOT + "units_db.json";

    static const size_t UPPER_LIMIT_PARTY = 100;
    static const int UPPER_LIMIT_HEALTH   = 100;
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<UnitAvailableNamesSingletonComponent> InitializeUnitAvailableNamesComponent()
{
    auto& resourceLoadingService = genesis::resources::ResourceLoadingService::GetInstance();
    const auto namesListResourceId = resourceLoadingService.LoadResource(NAMES_LIST_FILE_PATH);
    const auto& namesListDataResource = resourceLoadingService.GetResource<genesis::resources::DataFileResource>(namesListResourceId);
    
    auto unitAvailableNamesComponent = std::make_unique<UnitAvailableNamesSingletonComponent>();
    unitAvailableNamesComponent->mAvailableUnitNamesList = StringSplit(namesListDataResource.GetContents(), ',');
    return unitAvailableNamesComponent;
}

///------------------------------------------------------------------------------------------------

void LoadUnitBaseStats()
{
    auto unitBaseStatsComponent = std::make_unique<UnitBaseStatsSingletonComponent>();
    auto& resourceLoadingService = genesis::resources::ResourceLoadingService::GetInstance();
    
    // Get unit base stats file resource
    auto resourceId = resourceLoadingService.LoadResource(UNIT_BASE_STATS_FILE_PATH);
    const auto& unitBaseStatsResource = resourceLoadingService.GetResource<genesis::resources::DataFileResource>(resourceId);

    // Parse unit base stats
    const auto unitBaseStatsJson = nlohmann::json::parse(unitBaseStatsResource.GetContents());
    for (auto iter = unitBaseStatsJson.cbegin(); iter != unitBaseStatsJson.end(); ++iter)
    {
        auto unitTypeName  = StringId(iter.key());
        auto unitBaseStats = iter.value();
        
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mUnitModelName = StringId(unitBaseStats["model_name"].get<std::string>());
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mUnitModelScaleFactor = unitBaseStats["model_scale_factor"].get<float>();
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mAttackAnimationDamageTrigger = unitBaseStats["attack_animation_damage_trigger"].get<float>();
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mBaseDamage = unitBaseStats["base_damage"].get<int>();
    }
    
    genesis::ecs::World::GetInstance().SetSingletonComponent<UnitBaseStatsSingletonComponent>(std::move(unitBaseStatsComponent));
}

///------------------------------------------------------------------------------------------------

StringId GetRandomAvailableUnitName()
{
    auto& world = genesis::ecs::World::GetInstance();
    
    if (!world.HasSingletonComponent<UnitAvailableNamesSingletonComponent>())
    {
        world.SetSingletonComponent<UnitAvailableNamesSingletonComponent>(InitializeUnitAvailableNamesComponent());
    }
    
    auto& globalUnitInfoComponent = world.GetSingletonComponent<UnitAvailableNamesSingletonComponent>();
    const auto randomIndex = genesis::math::RandomInt(0, globalUnitInfoComponent.mAvailableUnitNamesList.size() - 1);
    return StringId(globalUnitInfoComponent.mAvailableUnitNamesList.at(randomIndex));
}

///-----------------------------------------------------------------------------------------------

StringId GetUnitModelName(const StringId unitTypeName)
{
    return genesis::ecs::World::GetInstance().GetSingletonComponent<UnitBaseStatsSingletonComponent>().mUnitTypeNameToBaseStats.at(unitTypeName).mUnitModelName;
}

///-----------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitPartyColor(const UnitStatsComponent& unitStatsComponent)
{
    const auto partyFillPercent = 180.0 * static_cast<float>(unitStatsComponent.mParty.size())/(genesis::math::Max(unitStatsComponent.mParty.size(), UPPER_LIMIT_PARTY));
    
    genesis::colors::HsvTriplet<float> hsvTarget(180.0f - partyFillPercent, 0.8f, 0.5f);
    return genesis::colors::HsvToRgb(hsvTarget);
    
}

///-----------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitHealthColor(const UnitStatsComponent& unitStatsComponent)
{
    const auto healthFillPercent = 1.0 * static_cast<float>(unitStatsComponent.mStats.mHealth)/(genesis::math::Max(unitStatsComponent.mStats.mHealth, UPPER_LIMIT_HEALTH));
    
    genesis::colors::HsvTriplet<float> hsvTarget(0.0f, 1.0f, healthFillPercent);
    return genesis::colors::HsvToRgb(hsvTarget);
}

///-----------------------------------------------------------------------------------------------
