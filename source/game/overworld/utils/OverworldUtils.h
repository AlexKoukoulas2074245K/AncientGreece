///------------------------------------------------------------------------------------------------
///  OverworldUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///------------------------------------------------------------------------------------------------

#ifndef OverworldUtils_h
#define OverworldUtils_h

///------------------------------------------------------------------------------------------------

#include "../../../engine/common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

void PopulateOverworldEntities();

///------------------------------------------------------------------------------------------------

void DestroyOverworldEntities();

///------------------------------------------------------------------------------------------------

float GetTerrainHeightAtPosition(const glm::vec3& position);

///------------------------------------------------------------------------------------------------

float GetTerrainSpeedMultiplierAtPosition(const glm::vec3& position);

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* OverworldUtils_h */
