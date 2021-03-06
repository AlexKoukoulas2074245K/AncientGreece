///------------------------------------------------------------------------------------------------
///  UnitStatsComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef UnitStats_h
#define UnitStats_h

///-----------------------------------------------------------------------------------------------

#include "overworld/AreaTypes.h"
#include "overworld/utils/TimeStamp.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

struct UnitStats
{
    tsl::robin_map<StringId, overworld::TimeStamp, StringIdHasher> mUnitEventTimestamps;
    StringId mUnitName = StringId();
    StringId mUnitType = StringId();
    StringId mUnitModelName = StringId();
    
    int mHealth = 0;
    int mDamage = 0;
    float mUnitModelScaleFactor = 0.0f;
    float mAttackAnimationDamageTrigger = 0.0f;
    float mSpeedMultiplier = 0.0f;
    float mAttackRange = 0.0f;
    float mCurrentRestingDuration = 0.0f;
    float mCurrentBattleDuration = 0.0f;
    bool mIsRangedUnit = false;
    overworld::AreaTypeMask mNavigableAreaTypes = overworld::areaTypeMasks::NEUTRAL | overworld::areaTypeMasks::SEA | overworld::areaTypeMasks::MOUNTAIN | overworld::areaTypeMasks::HIGH_MOUNTAIN | overworld::areaTypeMasks::FOREST;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitStats_h */
