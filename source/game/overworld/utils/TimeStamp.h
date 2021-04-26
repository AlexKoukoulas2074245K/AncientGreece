///------------------------------------------------------------------------------------------------
///  TimeStamp.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///------------------------------------------------------------------------------------------------

#ifndef TimeStamp_h
#define TimeStamp_h

///------------------------------------------------------------------------------------------------

#include "../../../engine/common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

struct TimeStamp
{
public:
    TimeStamp(): mYearBc(500), mDay(1), mTimeDtAccum(0.0f) {}
    TimeStamp(const int yearBc, const int day, const float timeDtAccum): mYearBc(yearBc), mDay(day), mTimeDtAccum(timeDtAccum) {}
    
    inline float GetTime() const
    {
        const auto dayDuration = 2.0f * genesis::math::PI;
        return (500 - mYearBc) * (dayDuration * 365.0f) + mDay * dayDuration + mTimeDtAccum;
    }
    
    int mYearBc;
    int mDay;
    float mTimeDtAccum;
};

///------------------------------------------------------------------------------------------------

inline bool operator == (const TimeStamp& lhs, const TimeStamp& rhs)
{
    return lhs.mYearBc == rhs.mYearBc && lhs.mDay == rhs.mDay && genesis::math::Abs(lhs.mTimeDtAccum - rhs.mTimeDtAccum) <= genesis::math::EQ_THRESHOLD;
}

///------------------------------------------------------------------------------------------------

inline bool operator > (const TimeStamp& lhs, const TimeStamp& rhs)
{
    if (lhs.mYearBc < rhs.mYearBc) return true;
    if (lhs.mDay > rhs.mDay) return true;
    return lhs.mTimeDtAccum > rhs.mTimeDtAccum;
}

///------------------------------------------------------------------------------------------------

inline float operator - (const TimeStamp& lhs, const TimeStamp& rhs)
{
    return lhs.GetTime() - rhs.GetTime();
}

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* OverworldUtils_h */
