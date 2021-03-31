///------------------------------------------------------------------------------------------------
///  UnitCollisionUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef UnitCollisionUtils_h
#define UnitCollisionUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

void AddCollidableDataToUnit(const genesis::ecs::EntityId unitEntity);

///------------------------------------------------------------------------------------------------

bool AreEntitiesColliding(const genesis::ecs::EntityId entityA, const genesis::ecs::EntityId entityB);

///------------------------------------------------------------------------------------------------

#endif /* UnitCollisionUtils_h */
