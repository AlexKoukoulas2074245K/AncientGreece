///------------------------------------------------------------------------------------------------
///  CityStateInfoUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/03/2021.
///------------------------------------------------------------------------------------------------

#include "CityStateInfoUtils.h"
#include "../components/CityStateInfoSingletonComponent.h"
#include "../../engine/ECS.h"
#include "../../engine/resources/DataFileResource.h"
#include "../../engine/resources/ResourceLoadingService.h"

#include <json.hpp>

///------------------------------------------------------------------------------------------------

namespace
{
    static const std::string CITY_STATE_INFO_FILE_PATH = genesis::resources::ResourceLoadingService::RES_DATA_ROOT + "city_state_db.json";
    static const int UPPER_LIMIT_GARISSON = 1100;
    static const int UPPER_LIMIT_RENOWN   = 100;
}

///------------------------------------------------------------------------------------------------

void LoadCityStateInfo()
{
    auto cityStateInfoComponent = std::make_unique<CityStateInfoSingletonComponent>();
    auto& resourceLoadingService = genesis::resources::ResourceLoadingService::GetInstance();
    
    // Get city state info file resource
    auto resourceId = resourceLoadingService.LoadResource(CITY_STATE_INFO_FILE_PATH);
    const auto& cityStateInfoResource = resourceLoadingService.GetResource<genesis::resources::DataFileResource>(resourceId);

    // Parse city state info
    const auto cityStateInfoJson = nlohmann::json::parse(cityStateInfoResource.GetContents());
    for (auto iter = cityStateInfoJson.cbegin(); iter != cityStateInfoJson.end(); ++iter)
    {
        auto cityStateName = StringId(iter.key());
        auto cityStateInfo = iter.value();
        
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mPosition = glm::vec3
        (
            cityStateInfo["x"].get<float>(),
            cityStateInfo["y"].get<float>(),
            0.0f
        );
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mRenown = cityStateInfo["renown"].get<int>();
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mGarisson = cityStateInfo["garisson"].get<int>();
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mDescription = cityStateInfo["description"].get<std::string>();
    }
    
    genesis::ecs::World::GetInstance().SetSingletonComponent<CityStateInfoSingletonComponent>(std::move(cityStateInfoComponent));
}

///-----------------------------------------------------------------------------------------------

CityStateInfo& GetCityStateInfo(const StringId cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    return world.GetSingletonComponent<CityStateInfoSingletonComponent>().mCityStateNameToInfo.at(cityStateName);
}

///-----------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetCityStateGarissonColor(const StringId cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto garisson =  world.GetSingletonComponent<CityStateInfoSingletonComponent>().mCityStateNameToInfo.at(cityStateName).mGarisson;
    const auto garissonFillPercent = 180.0 * static_cast<float>(garisson)/(genesis::math::Max(garisson, UPPER_LIMIT_GARISSON));
    
    genesis::colors::HsvTriplet<float> hsvTarget(180.0f - garissonFillPercent, 0.8f, 0.5f);
    return genesis::colors::HsvToRgb(hsvTarget);
}

///-----------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetCityStateRenownColor(const StringId cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto renown =  world.GetSingletonComponent<CityStateInfoSingletonComponent>().mCityStateNameToInfo.at(cityStateName).mRenown;
    const auto renownFillPercent = 180.0 * static_cast<float>(renown)/(genesis::math::Max(renown, UPPER_LIMIT_RENOWN));
    
    genesis::colors::HsvTriplet<float> hsvTarget(180.0f - renownFillPercent, 0.8f, 0.5f);
    return genesis::colors::HsvToRgb(hsvTarget);
}

///-----------------------------------------------------------------------------------------------