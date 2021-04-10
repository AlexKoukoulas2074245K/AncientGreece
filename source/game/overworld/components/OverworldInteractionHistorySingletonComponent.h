///------------------------------------------------------------------------------------------------
///  OverworldInteractionHistorySingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 10/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldInteractionHistorySingletonComponent_h
#define OverworldInteractionHistorySingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "OverworldInteractionComponent.h"
#include "../../engine/ECS.h"
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldInteractionHistorySingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<InteractionInfo> mInteractions;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldUnitInteractionHistorySingletonComponent_h */
