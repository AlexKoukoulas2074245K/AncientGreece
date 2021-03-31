///------------------------------------------------------------------------------------------------
///  OverworldBattleProcessingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldBattleProcessingSystem.h"
#include "../components/OverworldUnitInteractionHistorySingletonComponent.h"
#include "../utils/OverworldInteractionUtils.h"
#include "../utils/OverworldCityStateUtils.h"
#include "../../battle/utils/BattleUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../GameContexts.h"
#include "../../components/CollidableComponent.h"
#include "../../utils/UnitFactoryUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../view/components/ViewQueueSingletonComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/MeshResource.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId ATTACK_EVENT_NAME = StringId("attack");
    static const StringId UNIT_PREVIEW_POPUP_NAME = StringId("unit_preview_popup");
}

///-----------------------------------------------------------------------------------------------

OverworldBattleProcessingSystem::OverworldBattleProcessingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldBattleProcessingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>&) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& viewQueueComponent = world.GetSingletonComponent<view::ViewQueueSingletonComponent>();
    
    if (viewQueueComponent.mLastViewDestructionEvent == ATTACK_EVENT_NAME)
    {
        viewQueueComponent.mLastViewDestructionEvent = StringId();
        const auto& lastInteraction = GetLastUnitInteraction();
        
        const auto& attackingSideStatsComponent = world.GetComponent<UnitStatsComponent>(lastInteraction.mInstigatorEntityId);
        auto attackingSideParty = attackingSideStatsComponent.mParty;
        attackingSideParty.push_back(attackingSideStatsComponent.mStats);
        
        const auto& defendingSideStatsComponent = world.GetComponent<UnitStatsComponent>(lastInteraction.mOtherEntityId);
        auto defendingSideParty = defendingSideStatsComponent.mParty;
        defendingSideParty.push_back(defendingSideStatsComponent.mStats);
        
        battle::QueueBattle(lastInteraction.mInstigatorEntityId, lastInteraction.mOtherEntityId);
        
        RemoveOverworldCityStates();
        
        world.DestroyEntity(world.FindEntityWithName(StringId("map")));
        world.DestroyEntity(world.FindEntityWithName(StringId("map_edge_1")));
        world.DestroyEntity(world.FindEntityWithName(StringId("map_edge_2")));
        world.DestroyEntity(world.FindEntityWithName(StringId("map_edge_3")));
        world.DestroyEntity(world.FindEntityWithName(StringId("map_edge_4")));
        world.DestroyEntities(world.FindAllEntitiesWithName(UNIT_PREVIEW_POPUP_NAME));
        world.DestroyEntities(world.FindAllEntitiesWithName(StringId("overworld_unit")));
        world.DestroyEntities(world.FindAllEntitiesWithName(StringId("player")));
        
        genesis::rendering::LoadAndCreateStaticModelByName("battle_grass", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("battle_field"));
        
        world.ChangeContext(BATTLE_CONTEXT);
        
        battle::PrepareBattleCamera();
        
        auto xCounter = 0.0f;
        auto yCounter = 0.0f;
        auto unitCounter = 0;
        for (const auto& unitStats: attackingSideParty)
        {
            auto unitEntity = CreateUnit(unitStats.mUnitType, unitStats.mUnitName, StringId("battle_unit"), unitStats.mSpeedMultiplier, glm::vec3(xCounter, yCounter, 0.0f));
            
            const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(unitEntity);
            const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(unitEntity);
            const auto& meshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
            
            xCounter += meshResource.GetDimensions().x * transformComponent.mScale.x;
            
            if (++unitCounter > 10)
            {
                unitCounter = 0;
                xCounter = 0;
                yCounter -= meshResource.GetDimensions().y * transformComponent.mScale.y;
            }
        }
        
        xCounter = 0.0f;
        yCounter = 0.1f;
        unitCounter = 0;
        for (const auto& unitStats: defendingSideParty)
        {
            auto unitEntity = CreateUnit(unitStats.mUnitType, unitStats.mUnitName, StringId("battle_unit"), unitStats.mSpeedMultiplier, glm::vec3(xCounter, yCounter, 0.0f));
            const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(unitEntity);
            const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(unitEntity);
            const auto& meshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
            
            xCounter += meshResource.GetDimensions().x * transformComponent.mScale.x;
            
            if (++unitCounter > 10)
            {
                unitCounter = 0;
                xCounter = 0;
                yCounter += meshResource.GetDimensions().y * transformComponent.mScale.y;
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------
