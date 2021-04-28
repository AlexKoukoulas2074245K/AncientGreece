///------------------------------------------------------------------------------------------------
///  CityStateInfoUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef CityStateInfoUtils_h
#define CityStateInfoUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/common/utils/ColorUtils.h"
#include "../../engine/common/utils/StringUtils.h"

///------------------------------------------------------------------------------------------------

struct CityStateInfo;

///------------------------------------------------------------------------------------------------

void LoadCityStateInfo();

///------------------------------------------------------------------------------------------------

float GetCityStateNameSize(const StringId& cityStateName);

///------------------------------------------------------------------------------------------------

CityStateInfo& GetCityStateInfo(const StringId& cityStateName);

///------------------------------------------------------------------------------------------------

StringId GetRulingCityStateOfUnit(const StringId& unitName);

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetCityStateGarissonColor(const StringId& cityStateName);

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetCityStateRenownColor(const StringId& cityStateName);

///------------------------------------------------------------------------------------------------

#endif /* CityStateInfoUtils_h */
