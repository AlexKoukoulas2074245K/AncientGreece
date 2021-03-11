///------------------------------------------------------------------------------------------------
///  ColorUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 11/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ColorUtils_h
#define ColorUtils_h

///-----------------------------------------------------------------------------------------------

#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace colors
{

///-----------------------------------------------------------------------------------------------

struct RGBTriplet
{
    RGBTriplet(const int r, const int g, const int b)
        : mRed(r)
        , mGreen(g)
        , mBlue(b)
    {
    }
    
    const int mRed;
    const int mGreen;
    const int mBlue;
};

///-----------------------------------------------------------------------------------------------
/// Custom les operator for RGBTriplet to be used indirectly by stl containers
inline bool operator < (const RGBTriplet& lhs, const RGBTriplet& rhs)
{
    int lhsRGB = lhs.mRed; lhsRGB = (lhsRGB << 8) + lhs.mGreen; lhsRGB = (lhsRGB << 8) + lhs.mBlue;
    int rhsRGB = rhs.mRed; rhsRGB = (rhsRGB << 8) + rhs.mGreen; rhsRGB = (rhsRGB << 8) + rhs.mBlue;
    return lhsRGB < rhsRGB;
}

///-----------------------------------------------------------------------------------------------
/// Custom equality operator for RGBTriplet to be used indirectly by stl containers
inline bool operator == (const RGBTriplet& lhs, const RGBTriplet& rhs)
{
    return lhs.mRed == rhs.mRed && lhs.mGreen == rhs.mGreen && lhs.mBlue == rhs.mBlue;
}

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ColorUtils_h */
