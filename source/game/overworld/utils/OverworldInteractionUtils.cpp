///------------------------------------------------------------------------------------------------
///  OverworldInteractionUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldInteractionUtils.h"
#include "../components/OverworldInteractionHistorySingletonComponent.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

void SaveInteractionToHistory(const genesis::ecs::EntityId instigatorEntityId, const genesis::ecs::EntityId otherEntityId, const StringId& instigatorEntityName, const StringId& otherEntityName)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<OverworldInteractionHistorySingletonComponent>())
    {
        world.SetSingletonComponent<OverworldInteractionHistorySingletonComponent>(std::make_unique<OverworldInteractionHistorySingletonComponent>());
    }
    
    auto& interactionHistoryComponent = world.GetSingletonComponent<OverworldInteractionHistorySingletonComponent>();
    interactionHistoryComponent.mInteractions.push_back({instigatorEntityId, otherEntityId, instigatorEntityName, otherEntityName});
}

///-----------------------------------------------------------------------------------------------

const InteractionInfo& GetLastInteraction()
{
    return genesis::ecs::World::GetInstance().GetSingletonComponent<OverworldInteractionHistorySingletonComponent>().mInteractions.back();
}

///-----------------------------------------------------------------------------------------------

}

