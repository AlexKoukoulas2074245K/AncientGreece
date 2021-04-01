///------------------------------------------------------------------------------------------------
///  OverworldInteractionUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef OverworldInteractionUtils_h
#define OverworldInteractionUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

struct UnitInteraction;

///------------------------------------------------------------------------------------------------

void SaveInteractionToHistory(const genesis::ecs::EntityId instigatorEntityId, const genesis::ecs::EntityId otherEntityId, const StringId instigatorEntityName, const StringId otherEntityName);

///------------------------------------------------------------------------------------------------

const UnitInteraction& GetLastUnitInteraction();

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* OverworldInteractionUtils_h */
