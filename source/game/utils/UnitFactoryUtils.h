///------------------------------------------------------------------------------------------------
///  UnitFactoryUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef UnitFactoryUtils_h
#define UnitFactoryUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId CreateUnit(const StringId unitTypeName, const StringId unitName = StringId(), const StringId entityName = StringId(), const float speedMultipiler = 1.0f, const glm::vec3& position = glm::vec3(), const glm::vec3& rotation = glm::vec3(), const glm::vec3 scale = glm::vec3(1.0f));

///------------------------------------------------------------------------------------------------

#endif /* UnitFactoryUtils_h */
