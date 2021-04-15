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
#include "../../../engine/ECS.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetMapEntity();

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetPlayerEntity();

///------------------------------------------------------------------------------------------------

StringId GetPlayerEntityName();

///------------------------------------------------------------------------------------------------

StringId GetGenericOverworldUnitEntityName();

///------------------------------------------------------------------------------------------------

void PopulateOverworldEntities();

///------------------------------------------------------------------------------------------------

void PrepareOverworldCamera();

///------------------------------------------------------------------------------------------------

void DestroyOverworldEntities();

///------------------------------------------------------------------------------------------------

void SaveOverworldStateToFile();

///------------------------------------------------------------------------------------------------

bool TryLoadOverworldStateFromFile();

///------------------------------------------------------------------------------------------------

float GetTerrainSpeedMultiplierAtPosition(const glm::vec3& position);

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* OverworldUtils_h */
