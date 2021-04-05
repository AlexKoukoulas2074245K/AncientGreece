///------------------------------------------------------------------------------------------------
///  BattleUnitCollisionComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleUnitCollisionComponent_h
#define BattleUnitCollisionComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

class BattleUnitCollisionComponent final: public genesis::ecs::IComponent
{
public:
    genesis::ecs::EntityId mFirstEntity = genesis::ecs::NULL_ENTITY_ID;
    genesis::ecs::EntityId mSecondEntity = genesis::ecs::NULL_ENTITY_ID;
    float mPenetration = 0.0f;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* BattleUnitCollisionComponent_h */
