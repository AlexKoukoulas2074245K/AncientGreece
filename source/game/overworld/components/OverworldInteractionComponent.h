///------------------------------------------------------------------------------------------------
///  OverworldInteractionComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 10/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldInteractionComponent_h
#define OverworldInteractionComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

struct InteractionInfo
{
    genesis::ecs::EntityId mInstigatorEntityId = genesis::ecs::NULL_ENTITY_ID;
    genesis::ecs::EntityId mOtherEntityId = genesis::ecs::NULL_ENTITY_ID;
    StringId mInstigatorEntityName = StringId();
    StringId mOtherEntityName = StringId();
};

///-----------------------------------------------------------------------------------------------

class OverworldInteractionComponent final: public genesis::ecs::IComponent
{
public:
    InteractionInfo mInteraction;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldTargetComponent_h */
