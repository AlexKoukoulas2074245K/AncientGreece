///------------------------------------------------------------------------------------------------
///  OverworldUnitInteractionHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldUnitInteractionHandlingSystem.h"
#include "../components/OverworldUnitInteractionComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

OverworldUnitInteractionHandlingSystem::OverworldUnitInteractionHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldUnitInteractionHandlingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& overworldUnitInteractionComponent = world.GetComponent<OverworldUnitInteractionComponent>(entityId);
        
        if (world.HasEntity(overworldUnitInteractionComponent.mInstigatorEntityId) && world.HasEntity(overworldUnitInteractionComponent.mOtherEntityId))
        {
            
        }
        
        world.DestroyEntity(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

}
