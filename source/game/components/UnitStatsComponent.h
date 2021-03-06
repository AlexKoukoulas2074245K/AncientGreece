///------------------------------------------------------------------------------------------------
///  UnitStatsComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef UnitStatsComponent_h
#define UnitStatsComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../UnitStats.h"

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<UnitStats> mParty;
    UnitStats mStats;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitStatsComponent_h */
