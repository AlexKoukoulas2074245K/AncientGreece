///------------------------------------------------------------------------------------------------
///  OverworldInteractionUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldInteractionUtils.h"
#include "../components/OverworldUnitInteractionHistorySingletonComponent.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

void SaveInteractionToHistory(const genesis::ecs::EntityId instigatorEntityId, const genesis::ecs::EntityId otherEntityId, const StringId instigatorEntityName, const StringId otherEntityName)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<OverworldUnitInteractionHistorySingletonComponent>())
    {
        world.SetSingletonComponent<OverworldUnitInteractionHistorySingletonComponent>(std::make_unique<OverworldUnitInteractionHistorySingletonComponent>());
    }
    
    auto& unitInteractionHistoryComponent = world.GetSingletonComponent<OverworldUnitInteractionHistorySingletonComponent>();
    unitInteractionHistoryComponent.mUnitInteractions.push_back({instigatorEntityId, otherEntityId, instigatorEntityName, otherEntityName});
}

///-----------------------------------------------------------------------------------------------

const UnitInteraction& GetLastUnitInteraction()
{
    return genesis::ecs::World::GetInstance().GetSingletonComponent<OverworldUnitInteractionHistorySingletonComponent>().mUnitInteractions.back();
}

///-----------------------------------------------------------------------------------------------

}

