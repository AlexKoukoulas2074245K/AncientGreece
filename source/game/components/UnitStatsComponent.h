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

class UnitStatsComponent final: public genesis::ecs::IComponent
{
public:
    StringId mUnitName = StringId();
    int mPartySize = 0;
    float mSpeedMultiplier = 1.0f;
    overworld::AreaTypeMask mNavigableAreaTypes = overworld::areaTypeMasks::NEUTRAL | overworld::areaTypeMasks::SEA | overworld::areaTypeMasks::MOUNTAIN | overworld::areaTypeMasks::HIGH_MOUNTAIN | overworld::areaTypeMasks::FOREST;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitStatsComponent_h */
