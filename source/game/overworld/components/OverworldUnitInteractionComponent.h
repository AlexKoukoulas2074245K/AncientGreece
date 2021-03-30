///------------------------------------------------------------------------------------------------
///  OverworldUnitInteractionComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldUnitInteractionComponent_h
#define OverworldUnitInteractionComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldUnitInteractionComponent final: public genesis::ecs::IComponent
{
public:
    genesis::ecs::EntityId mInstigatorEntityId = genesis::ecs::NULL_ENTITY_ID;
    genesis::ecs::EntityId mOtherEntityId = genesis::ecs::NULL_ENTITY_ID;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldTargetComponent_h */
