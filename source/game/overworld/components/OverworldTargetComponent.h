///------------------------------------------------------------------------------------------------
///  OverworldTargetComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 11/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldTargetComponent_h
#define OverworldTargetComponent_h

///-----------------------------------------------------------------------------------------------

#include "../AreaTypes.h"
#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/common/utils/ColorUtils.h"

#include <list>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldTargetComponent final: public genesis::ecs::IComponent
{
public:
    genesis::ecs::EntityId mOptionalEntityTarget = genesis::ecs::NULL_ENTITY_ID;
    std::list<glm::vec3> mTargetPositionPath;
    AreaTypeMask mTargetAreaType;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldTargetComponent_h */
