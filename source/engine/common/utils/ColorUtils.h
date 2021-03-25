///------------------------------------------------------------------------------------------------
///  ColorUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 11/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ColorUtils_h
#define ColorUtils_h

///-----------------------------------------------------------------------------------------------

#include "MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace colors
{

///-----------------------------------------------------------------------------------------------

static const glm::vec4 BLACK   = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
static const glm::vec4 GRAY    = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
static const glm::vec4 WHITE   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
static const glm::vec4 RED     = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static const glm::vec4 GREEN   = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static const glm::vec4 BLUE    = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static const glm::vec4 MAGENTA = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
static const glm::vec4 YELLOW  = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
static const glm::vec4 CYAN    = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

///-----------------------------------------------------------------------------------------------

template <class T>
struct RgbTriplet
{
    RgbTriplet(const T r, const T g, const T b)
        : mRed(r)
        , mGreen(g)
        , mBlue(b)
    {
    }
    
    RgbTriplet()
        : mRed()
        , mGreen()
        , mBlue()
    {
    }
    
    T mRed;
    T mGreen;
    T mBlue;
};

///-----------------------------------------------------------------------------------------------
/// Custom les operator for RGBTriplet to be used indirectly by stl containers
template <class T>
inline bool operator < (const RgbTriplet<T>& lhs, const RgbTriplet<T>& rhs)
{
    int lhsRGB = lhs.mRed; lhsRGB = (lhsRGB << 8) + lhs.mGreen; lhsRGB = (lhsRGB << 8) + lhs.mBlue;
    int rhsRGB = rhs.mRed; rhsRGB = (rhsRGB << 8) + rhs.mGreen; rhsRGB = (rhsRGB << 8) + rhs.mBlue;
    return lhsRGB < rhsRGB;
}

///-----------------------------------------------------------------------------------------------
/// Custom equality operator for RGBTriplet to be used indirectly by stl containers
template <class T>
inline bool operator == (const RgbTriplet<T>& lhs, const RgbTriplet<T>& rhs)
{
    return lhs.mRed == rhs.mRed && lhs.mGreen == rhs.mGreen && lhs.mBlue == rhs.mBlue;
}

///-----------------------------------------------------------------------------------------------

template <class T>
struct HsvTriplet
{
    HsvTriplet(const T h, const T s, const T v)
        : mHue(h)
        , mSat(s)
        , mVal(v)
    {
    }
    
    HsvTriplet()
        : mHue()
        , mSat()
        , mVal()
    {
        
    }
    
    T mHue;
    T mSat;
    T mVal;
};

///-----------------------------------------------------------------------------------------------
/// Custom les operator for RGBTriplet to be used indirectly by stl containers
template <class T>
inline bool operator < (const HsvTriplet<T>& lhs, const HsvTriplet<T>& rhs)
{
    int lhsHsv = lhs.mHue; lhsHsv = (lhsHsv << 8) + lhs.mSat; lhsHsv = (lhsHsv << 8) + lhs.mVal;
    int rhsHsv = rhs.mHue; rhsHsv = (lhsHsv << 8) + rhs.mSat; lhsHsv = (lhsHsv << 8) + rhs.mVal;
    return lhsHsv < rhsHsv;
}

///-----------------------------------------------------------------------------------------------
/// Custom equality operator for RGBTriplet to be used indirectly by stl containers
template <class T>
inline bool operator == (const HsvTriplet<T>& lhs, const HsvTriplet<T>& rhs)
{
    return lhs.mHue == rhs.mHue && lhs.mSat == rhs.mSat && lhs.mVal == rhs.mVal;
}

///-----------------------------------------------------------------------------------------------
/// Calculates and returns the HSV triplet equivalent of the input RGB.
/// Input is expected to be in the range 0.0-1.0.
///
/// @param[in] rgb The rgb triplet (in range 0.0-1.0 to convert to hsv)
/// @returns the conversion result
HsvTriplet<float> RgbToHsv(const RgbTriplet<float> rgb);

///-----------------------------------------------------------------------------------------------
/// Calculates and returns the RGB triplet equivalent of the input HSV.
/// Input is expected to be in the range 0.0-1.0.
///
/// @param[in] rgb The rgb triplet (in range 0.0-1.0 to convert to hsv)
/// @returns the conversion result
RgbTriplet<float> HsvToRgb(const HsvTriplet<float> hsv);

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ColorUtils_h */
