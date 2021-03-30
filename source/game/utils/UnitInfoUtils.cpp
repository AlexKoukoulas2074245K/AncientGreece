///------------------------------------------------------------------------------------------------
///  UnitInfoUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/03/2021.
///------------------------------------------------------------------------------------------------

#include "UnitInfoUtils.h"
#include "../components/GlobalUnitInfoSingletonComponent.h"
#include "../components/UnitStatsComponent.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/resources/DataFileResource.h"
#include "../../engine/resources/ResourceLoadingService.h"

///------------------------------------------------------------------------------------------------

namespace
{
    static const std::string NAMES_LIST_FILE_PATH = genesis::resources::ResourceLoadingService::RES_DATA_ROOT + "names_list.dat";

    static const size_t UPPER_LIMIT_PARTY = 100;
    static const int UPPER_LIMIT_HEALTH   = 100;
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<GlobalUnitInfoSingletonComponent> InitializeGlobalUnitInfoComponent()
{
    auto& resourceLoadingService = genesis::resources::ResourceLoadingService::GetInstance();
    const auto namesListResourceId = resourceLoadingService.LoadResource(NAMES_LIST_FILE_PATH);
    const auto& namesListDataResource = resourceLoadingService.GetResource<genesis::resources::DataFileResource>(namesListResourceId);
    
    auto globalUnitInfoComponent = std::make_unique<GlobalUnitInfoSingletonComponent>();
    globalUnitInfoComponent->mAvailableUnitNamesList = StringSplit(namesListDataResource.GetContents(), ',');
    return globalUnitInfoComponent;
}

///------------------------------------------------------------------------------------------------

StringId GetRandomAvailableUnitName()
{
    auto& world = genesis::ecs::World::GetInstance();
    
    if (!world.HasSingletonComponent<GlobalUnitInfoSingletonComponent>())
    {
        world.SetSingletonComponent<GlobalUnitInfoSingletonComponent>(InitializeGlobalUnitInfoComponent());
    }
    
    auto& globalUnitInfoComponent = world.GetSingletonComponent<GlobalUnitInfoSingletonComponent>();
    const auto randomIndex = genesis::math::RandomInt(0, globalUnitInfoComponent.mAvailableUnitNamesList.size() - 1);
    return StringId(globalUnitInfoComponent.mAvailableUnitNamesList.at(randomIndex));
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
