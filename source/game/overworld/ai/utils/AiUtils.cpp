///------------------------------------------------------------------------------------------------
///  AiUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///------------------------------------------------------------------------------------------------

#include "AiUtils.h"
#include "../components/OverworldUnitAiRegisteredActionsSingletonComponent.h"
#include "../../../../engine/ECS.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

namespace ai
{

///------------------------------------------------------------------------------------------------

size_t GetAiActionIndex(const std::shared_ptr<IAiAction>& aiAction)
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& registeredActionsComponent = world.GetSingletonComponent<OverworldUnitAiRegisteredActionsSingletonComponent>();
    const auto foundIter = std::find(registeredActionsComponent.mRegisteredActions.cbegin(), registeredActionsComponent.mRegisteredActions.cend(), aiAction);
    
    return foundIter != registeredActionsComponent.mRegisteredActions.cend() ? std::distance(registeredActionsComponent.mRegisteredActions.cbegin(), foundIter) : -1;
}

///-----------------------------------------------------------------------------------------------

}

}



