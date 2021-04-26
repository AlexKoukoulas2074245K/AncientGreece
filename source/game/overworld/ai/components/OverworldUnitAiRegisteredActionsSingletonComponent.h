///------------------------------------------------------------------------------------------------
///  OverworldUnitAiRegisteredActionsSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldUnitAiRegisteredActionsSingletonComponent_h
#define OverworldUnitAiRegisteredActionsSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../actions/IAiAction.h"
#include "../../engine/ECS.h"

#include <memory>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

class OverworldUnitAiRegisteredActionsSingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<std::shared_ptr<IAiAction>> mRegisteredActions;
};

///-----------------------------------------------------------------------------------------------
 
}

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldUnitAiRegisteredActionsSingletonComponent_h */
