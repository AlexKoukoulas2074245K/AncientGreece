///------------------------------------------------------------------------------------------------
///  BattleSideComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleSideComponent_h
#define BattleSideComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

class BattleSideComponent final: public genesis::ecs::IComponent
{
public:
    StringId mBattleSideLeaderUnitName = StringId();
    StringId mBattleSideAssistingLeaderUnitName = StringId();
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleSideComponent_h */
