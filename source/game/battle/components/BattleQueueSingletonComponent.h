///------------------------------------------------------------------------------------------------
///  BattleQueueSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleQueueSingletonComponent_h
#define BattleQueueSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include <queue>

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

struct BattleEntry
{
    genesis::ecs::EntityId mAttackingEntityId;
    genesis::ecs::EntityId mDefendingEntityId;
};

///-----------------------------------------------------------------------------------------------

class BattleQueueSingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::queue<BattleEntry> mQueuedBattles;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleQueueSingletonComponent_h */
