///------------------------------------------------------------------------------------------------
///  OverworldDayTimeSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 18/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldDayTimeSingletonComponent_h
#define OverworldDayTimeSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldDayTimeSingletonComponent final: public genesis::ecs::IComponent
{
public:
    StringId mCurrentPeriod;
    float mDtAccum     = 0.0f;
    int mCurrentDay    = 0;
    int mCurrentYearBc = 0;
    bool mPeriodChangeTick = false;
    bool mDayChangeTick = false;
    bool mYearChangeTick = false;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldDayTimeSingletonComponent_h */
