///------------------------------------------------------------------------------------------------
///  OverworldUnitInteractionHistorySingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldUnitInteractionHistorySingletonComponent_h
#define OverworldUnitInteractionHistorySingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "OverworldUnitInteractionComponent.h"
#include "../../engine/ECS.h"
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldUnitInteractionHistorySingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<UnitInteraction> mUnitInteractions;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldUnitInteractionHistorySingletonComponent_h */
