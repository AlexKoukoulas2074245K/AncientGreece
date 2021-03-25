///------------------------------------------------------------------------------------------------
///  AreaTypes.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 22/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef AreaTypes_h
#define AreaTypes_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/common/utils/ColorUtils.h"

#include <map>

///-----------------------------------------------------------------------------------------------

namespace overworld
{
    
///-----------------------------------------------------------------------------------------------

using AreaTypeMask = int;
namespace areaTypeMasks
{
    static const AreaTypeMask NEUTRAL       = 0x01;
    static const AreaTypeMask BLOCKED       = 0x02;
    static const AreaTypeMask FOREST        = 0x04;
    static const AreaTypeMask SEA           = 0x08;
    static const AreaTypeMask MOUNTAIN      = 0x10;
    static const AreaTypeMask HIGH_MOUNTAIN = 0x20;
};

///-----------------------------------------------------------------------------------------------

const std::map<genesis::colors::RgbTriplet<int>, AreaTypeMask> RGB_TO_AREA_TYPE_MASK =
{
    { genesis::colors::RgbTriplet(255, 255, 255), areaTypeMasks::BLOCKED },
    { genesis::colors::RgbTriplet(  0,   0, 255), areaTypeMasks::SEA },
    { genesis::colors::RgbTriplet(119, 119, 119), areaTypeMasks::MOUNTAIN },
    { genesis::colors::RgbTriplet( 51,  51,  51), areaTypeMasks::HIGH_MOUNTAIN },
    { genesis::colors::RgbTriplet(  0, 255,   0), areaTypeMasks::FOREST }
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* AreaTypes_h */
