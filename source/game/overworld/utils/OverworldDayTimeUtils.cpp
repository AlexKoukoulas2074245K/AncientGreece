///------------------------------------------------------------------------------------------------
///  OverworldDayTimeUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldDayTimeUtils.h"
#include "../components/OverworldDayTimeSingletonComponent.h"
#include "../../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const float DAY_CYCLE_SPEED = 1.0f/20.0f;
}

///------------------------------------------------------------------------------------------------

TimeStamp GetCurrentTimestamp()
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& dayTimeComponent = world.GetSingletonComponent<OverworldDayTimeSingletonComponent>();
    
    return TimeStamp(dayTimeComponent.mCurrentYearBc, dayTimeComponent.mCurrentDay, dayTimeComponent.mTimeDtAccum);
}

///-----------------------------------------------------------------------------------------------

float GetHourDuration()
{
    return genesis::math::PI/12.0f;
}

///-----------------------------------------------------------------------------------------------

float GetDayDuration()
{
    return GetHourDuration() * 24.0f;
}

///-----------------------------------------------------------------------------------------------

float GetDayTimeSpeed()
{
    return DAY_CYCLE_SPEED;
}

///-----------------------------------------------------------------------------------------------

}



