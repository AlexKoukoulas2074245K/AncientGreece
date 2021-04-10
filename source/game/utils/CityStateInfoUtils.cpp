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

    static const float CITY_STATE_RX = -genesis::math::PI/2.0;
    static const float CITY_STATE_RY = 0.0f;

    static const float CITY_STATE_NAME_BASE_SIZE              = 0.001f;
    static const float CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER = 0.0001f;
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
            cityStateInfo["z"].get<float>()
        );
        
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mRotation = glm::vec3
        (
            CITY_STATE_RX,
            CITY_STATE_RY,
            cityStateInfo["rz"].get<float>()
        );
        
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mRenown = cityStateInfo["renown"].get<int>();
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mGarisson = cityStateInfo["garisson"].get<int>();
        cityStateInfoComponent->mCityStateNameToInfo[cityStateName].mDescription = cityStateInfo["description"].get<std::string>();
    }
    
    genesis::ecs::World::GetInstance().SetSingletonComponent<CityStateInfoSingletonComponent>(std::move(cityStateInfoComponent));
}

///-----------------------------------------------------------------------------------------------

float GetCityStateNameSize(const StringId& cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    return CITY_STATE_NAME_BASE_SIZE + CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER * cityStateInfoComponent.mCityStateNameToInfo.at(cityStateName).mRenown;
}

///-----------------------------------------------------------------------------------------------

CityStateInfo& GetCityStateInfo(const StringId& cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    return world.GetSingletonComponent<CityStateInfoSingletonComponent>().mCityStateNameToInfo.at(cityStateName);
}

///-----------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetCityStateGarissonColor(const StringId& cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto garisson =  world.GetSingletonComponent<CityStateInfoSingletonComponent>().mCityStateNameToInfo.at(cityStateName).mGarisson;
    const auto garissonFillPercent = 180.0 * static_cast<float>(garisson)/(genesis::math::Max(garisson, UPPER_LIMIT_GARISSON));
    
    genesis::colors::HsvTriplet<float> hsvTarget(180.0f - garissonFillPercent, 0.8f, 0.5f);
    return genesis::colors::HsvToRgb(hsvTarget);
}

///-----------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetCityStateRenownColor(const StringId& cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto renown =  world.GetSingletonComponent<CityStateInfoSingletonComponent>().mCityStateNameToInfo.at(cityStateName).mRenown;
    const auto renownFillPercent = 180.0 * static_cast<float>(renown)/(genesis::math::Max(renown, UPPER_LIMIT_RENOWN));
    
    genesis::colors::HsvTriplet<float> hsvTarget(180.0f - renownFillPercent, 0.8f, 0.5f);
    return genesis::colors::HsvToRgb(hsvTarget);
}

///-----------------------------------------------------------------------------------------------
