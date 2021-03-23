///------------------------------------------------------------------------------------------------
///  NavmapUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 22/03/2021.
///------------------------------------------------------------------------------------------------

#include "NavmapUtils.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

glm::ivec2 MapPositionToNavmapPixel(const glm::vec3& mapPosition, const glm::vec3& mapDimensions, const glm::ivec2& navmapDimensions)
{
    // Find relative displacement
    const auto relativeXDisplacement = mapPosition.x/(mapDimensions.x/2.0f);
    const auto relativeYDisplacement = mapPosition.y/(mapDimensions.y/2.0f);
    
    const auto targetPixelX = navmapDimensions.x/2 + relativeXDisplacement * navmapDimensions.x/2;
    const auto targetPixelY = navmapDimensions.y/2 - relativeYDisplacement * navmapDimensions.y/2;
    
    return glm::ivec2(targetPixelX, targetPixelY);
}

///-----------------------------------------------------------------------------------------------

glm::vec3 NavmapPixelToMapPosition(const glm::ivec2& pixelPosition, const glm::vec3& mapDimensions, const glm::ivec2& navmapDimensions)
{
    const auto halfNavMapWidth = navmapDimensions.x * 0.5f;
    const auto halfNavMapHeight = navmapDimensions.y * 0.5f;
    const auto halfMapWidth = mapDimensions.x * 0.5f;
    const auto halfMapHeight = mapDimensions.y * 0.5f;
    
    const auto targetWorldX = halfMapWidth * ((pixelPosition.x - halfNavMapWidth)/halfNavMapWidth);
    const auto targetWorldY = halfMapHeight * ((halfNavMapHeight - pixelPosition.y)/halfNavMapHeight);
    
    return glm::vec3(targetWorldX, targetWorldY, 0.0f);
}

///-----------------------------------------------------------------------------------------------

}

