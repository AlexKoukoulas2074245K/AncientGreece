///------------------------------------------------------------------------------------------------
///  UnitInfoUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef UnitInfoUtils_h
#define UnitInfoUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/ColorUtils.h"
#include "../../engine/common/utils/StringUtils.h"

#include <map>

///------------------------------------------------------------------------------------------------

struct UnitStats;
class UnitStatsComponent;

///------------------------------------------------------------------------------------------------

void LoadUnitBaseStats();

///------------------------------------------------------------------------------------------------

StringId GetRandomAvailableUnitName();

///------------------------------------------------------------------------------------------------

const UnitStats& GetUnitBaseStats(const StringId& unitTypeName);

///------------------------------------------------------------------------------------------------

std::map<StringId, int> GetUnitPartyCountBuckets(const UnitStatsComponent& unitStatsComponent, const bool includeLeader);

///------------------------------------------------------------------------------------------------

size_t GetUnitPartySize(const UnitStatsComponent& unitStatsComponent);

///------------------------------------------------------------------------------------------------

const StringId& GetUnitModelName(const StringId& unitTypeName);

///------------------------------------------------------------------------------------------------

bool IsUnitDead(const genesis::ecs::EntityId unitEntity);

///------------------------------------------------------------------------------------------------

std::string GetUnitCollectionString(const StringId& unitType, const int unitCount);

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitPartyColor(const UnitStatsComponent& unitStatsComponent);

///------------------------------------------------------------------------------------------------

genesis::colors::RgbTriplet<float> GetUnitHealthColor(const UnitStatsComponent& unitStatsComponent);

///------------------------------------------------------------------------------------------------

#endif /* UnitInfoUtils_h */
