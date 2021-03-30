///------------------------------------------------------------------------------------------------
///  UnitStatsComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef UnitStatsComponent_h
#define UnitStatsComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../overworld/AreaTypes.h"

///-----------------------------------------------------------------------------------------------

struct UnitStats
{
    StringId mUnitName = StringId();
    StringId mModelName = StringId();
    int mHealth = 100;
    int mDamage = 10;
    float mSpeedMultiplier = 1.0f;
    overworld::AreaTypeMask mNavigableAreaTypes = overworld::areaTypeMasks::NEUTRAL | overworld::areaTypeMasks::SEA | overworld::areaTypeMasks::MOUNTAIN | overworld::areaTypeMasks::HIGH_MOUNTAIN | overworld::areaTypeMasks::FOREST;
};

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<UnitStats> mParty;
    UnitStats mStats;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitStatsComponent_h */
