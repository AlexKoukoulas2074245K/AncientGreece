///------------------------------------------------------------------------------------------------
///  ColorUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 25/03/2021.
///-----------------------------------------------------------------------------------------------

#include "ColorUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace colors
{

///-----------------------------------------------------------------------------------------------
/// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
HsvTriplet<float> RgbToHsv(const RgbTriplet<float> rgb)
{
    HsvTriplet<float> hsv;
    float min, max, delta;

    min = rgb.mRed < rgb.mGreen ? rgb.mRed : rgb.mGreen;
    min = min      < rgb.mBlue  ? min      : rgb.mBlue;

    max = rgb.mRed > rgb.mGreen ? rgb.mRed : rgb.mGreen;
    max = max      > rgb.mBlue  ? max      : rgb.mBlue;

    hsv.mVal = max;                             // v
    delta = max - min;
    
    if (delta < 0.00001)
    {
        hsv.mSat = 0;
        hsv.mHue = 0; // undefined, maybe nan?
        return hsv;
    }
    if(max > 0.0)
    { // NOTE: if Max is == 0, this divide would cause a crash
        hsv.mSat = (delta / max);               // s
    }
    else
    {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        hsv.mSat = 0.0;
        hsv.mHue = NAN;                                      // its now undefined
        return hsv;
    }
    if(rgb.mRed >= max)                                      // > is bogus, just keeps compilor happy
        hsv.mHue = ( rgb.mGreen - rgb.mBlue ) / delta;       // between yellow & magenta
    else
    if( rgb.mGreen >= max )
        hsv.mHue = 2.0 + ( rgb.mBlue - rgb.mRed ) / delta;   // between cyan & yellow
    else
        hsv.mHue = 4.0 + ( rgb.mRed - rgb.mGreen ) / delta;  // between magenta & cyan

    hsv.mHue *= 60.0;                                        // degrees

    if( hsv.mHue < 0.0 )
        hsv.mHue += 360.0;

    return hsv;
}

///-----------------------------------------------------------------------------------------------
/// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
RgbTriplet<float> HsvToRgb(const HsvTriplet<float> hsv)
{
    float             hh, p, q, t, ff;
    long              i;
    RgbTriplet<float> rgb;

    if(hsv.mSat <= 0.0) // < is bogus, just shuts up warnings
    {
        rgb.mRed = hsv.mVal;
        rgb.mGreen = hsv.mVal;
        rgb.mBlue = hsv.mVal;
        return rgb;
    }
    
    hh = hsv.mHue;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = hsv.mVal * (1.0 - hsv.mSat);
    q = hsv.mVal * (1.0 - (hsv.mSat * ff));
    t = hsv.mVal * (1.0 - (hsv.mSat * (1.0 - ff)));

    switch(i)
    {
    case 0:
        rgb.mRed   = hsv.mVal;
        rgb.mGreen = t;
        rgb.mBlue  = p;
        break;
    case 1:
        rgb.mRed   = q;
        rgb.mGreen = hsv.mVal;
        rgb.mBlue  = p;
        break;
    case 2:
        rgb.mRed   = p;
        rgb.mGreen = hsv.mVal;
        rgb.mBlue  = t;
        break;

    case 3:
        rgb.mRed   = p;
        rgb.mGreen = q;
        rgb.mBlue  = hsv.mVal;
        break;
    case 4:
        rgb.mRed   = t;
        rgb.mGreen = p;
        rgb.mBlue  = hsv.mVal;
        break;
    case 5:
    default:
        rgb.mRed   = hsv.mVal;
        rgb.mGreen = p;
        rgb.mBlue  = q;
        break;
    }
    return rgb;
}

///-----------------------------------------------------------------------------------------------

glm::vec4 RgbTripletToVec4(const RgbTriplet<float> rgb)
{
    return glm::vec4(rgb.mRed, rgb.mGreen, rgb.mBlue, 1.0f);
}

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------
