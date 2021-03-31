///------------------------------------------------------------------------------------------------
///  UnitInfoUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef UnitInfoUtils_h
#define UnitInfoUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/common/utils/ColorUtils.h"
#include "../../engine/common/utils/StringUtils.h"

///------------------------------------------------------------------------------------------------

class UnitStatsComponent;

///------------------------------------------------------------------------------------------------

void LoadUnitBaseStats();

///------------------------------------------------------------------------------------------------

StringId GetRandomAvailableUnitName();

///------------------------------------------------------------------------------------------------

StringId GetUnitModelName(const StringId unitTypeName);

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitPartyColor(const UnitStatsComponent& unitStatsComponent);

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitHealthColor(const UnitStatsComponent& unitStatsComponent);

///------------------------------------------------------------------------------------------------

#endif /* UnitInfoUtils_h */
