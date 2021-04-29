///------------------------------------------------------------------------------------------------
///  OverworldBattleStateComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldBattleStateComponent_h
#define OverworldBattleStateComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldBattleStateComponent final: public genesis::ecs::IComponent
{
public:
    StringId mAttackingUnitName = StringId();
    StringId mDefendingUnitName = StringId();
    float mKillTimer = 0.0f;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldBattleStateComponent_h */
