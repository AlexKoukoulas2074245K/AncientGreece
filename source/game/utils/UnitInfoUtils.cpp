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
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mDamage = unitBaseStats["base_damage"].get<int>();
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mHealth = unitBaseStats["base_health"].get<int>();
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mSpeedMultiplier = unitBaseStats["base_speed_multiplier"].get<int>();
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mIsRangedUnit =
        unitBaseStats["is_ranged_unit"].get<bool>();
        
        if (unitBaseStats.count("attack_range"))
        {
            unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mAttackRange = unitBaseStats["attack_range"].get<float>();
        }
        
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mUnitType = unitTypeName;
        unitBaseStatsComponent->mUnitTypeNameToBaseStats[unitTypeName].mUnitName = unitTypeName;
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

const UnitStats& GetUnitBaseStats(const StringId& unitTypeName)
{
    return genesis::ecs::World::GetInstance().GetSingletonComponent<UnitBaseStatsSingletonComponent>().mUnitTypeNameToBaseStats.at(unitTypeName);
}

///-----------------------------------------------------------------------------------------------

std::map<StringId, int> GetUnitPartyCountBuckets(const UnitStatsComponent& unitStatsComponent, const bool includeLeader)
{
    std::map<StringId, int> resultBuckets;
    auto startingIndex = includeLeader ? 0U : 1U;
    
    for (auto i = startingIndex; i < unitStatsComponent.mParty.size(); ++i)
    {
        const auto& unitStats = unitStatsComponent.mParty[i];
        if (resultBuckets.count(unitStats.mUnitType) > 0)
        {
            resultBuckets[unitStats.mUnitType]++;
        }
        else
        {
            resultBuckets[unitStats.mUnitType] = 1;
        }
    }
    
    return resultBuckets;
}

///-----------------------------------------------------------------------------------------------

size_t GetUnitPartySize(const UnitStatsComponent& unitStatsComponent)
{
    // Plus one to include leader
    return unitStatsComponent.mParty.size();
}

///-----------------------------------------------------------------------------------------------

const StringId& GetUnitModelName(const StringId& unitTypeName)
{
    return genesis::ecs::World::GetInstance().GetSingletonComponent<UnitBaseStatsSingletonComponent>().mUnitTypeNameToBaseStats.at(unitTypeName).mUnitModelName;
}

///------------------------------------------------------------------------------------------------

bool IsUnitDead(const genesis::ecs::EntityId unitEntity)
{
    return genesis::ecs::World::GetInstance().GetComponent<UnitStatsComponent>(unitEntity).mStats.mHealth <= 0;
}

///------------------------------------------------------------------------------------------------

std::string GetUnitCollectionString(const StringId& unitType, const int unitCount)
{
    if (unitCount == 1)
    {
        return unitType.GetString();
    }
    else
    {
        if (StringEndsWith(unitType.GetString(), "man"))
        {
            return unitType.GetString().substr(0, unitType.GetString().size() - 3) + "men";
        }
        else
        {
            return unitType.GetString() + "s";
        }
    }
}

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitPartyColor(const UnitStatsComponent& unitStatsComponent)
{
    const auto unitPartySize = GetUnitPartySize(unitStatsComponent);
    const auto partyFillPercent = 180.0 * static_cast<float>(unitPartySize)/(genesis::math::Max(unitPartySize, UPPER_LIMIT_PARTY));
    
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
