///------------------------------------------------------------------------------------------------
///  OverworldUnitAiUpdaterSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/04/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldUnitAiUpdaterSystem.h"
#include "../actions/PatrolAroundRulingTownAction.h"
#include "../actions/RestAiAction.h"
#include "../actions/VisitRandomCityStateAiAction.h"
#include "../components/OverworldUnitAiComponent.h"
#include "../components/OverworldUnitAiRegisteredActionsSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

namespace
{
}

///-----------------------------------------------------------------------------------------------

OverworldUnitAiUpdaterSystem::OverworldUnitAiUpdaterSystem()
    : BaseSystem()
{
    auto registeredActionsComponent = std::make_unique<OverworldUnitAiRegisteredActionsSingletonComponent>();
    registeredActionsComponent->mRegisteredActions.push_back(std::make_unique<RestAiAction>());
    registeredActionsComponent->mRegisteredActions.push_back(std::make_unique<PatrolAroundRulingTownAction>());
    registeredActionsComponent->mRegisteredActions.push_back(std::make_unique<VisitRandomCityStateAiAction>());
    
    genesis::ecs::World::GetInstance().SetSingletonComponent<OverworldUnitAiRegisteredActionsSingletonComponent>(std::move(registeredActionsComponent));
}

///-----------------------------------------------------------------------------------------------

void OverworldUnitAiUpdaterSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto entitiesToProcess = entities;
    const auto& registeredActionsComponent = world.GetSingletonComponent<OverworldUnitAiRegisteredActionsSingletonComponent>();
    
    for (const auto& entityId: entitiesToProcess)
    {
        auto& aiComponent = world.GetComponent<OverworldUnitAiComponent>(entityId);
        
        if (aiComponent.mCurrentAction == nullptr)
        {
            if (aiComponent.mLastActionIndex != -1)
            {
                aiComponent.mCurrentAction = registeredActionsComponent.mRegisteredActions[aiComponent.mLastActionIndex];
                aiComponent.mLastActionIndex = -1;
            }
            else
            {
                // Get highest applicability action for current entity
                auto registeredActionsCopy = registeredActionsComponent.mRegisteredActions;
                
                std::shuffle(registeredActionsCopy.begin(), registeredActionsCopy.end(), genesis::math::GetRandomEngine());
                std::sort(registeredActionsCopy.begin(), registeredActionsCopy.end(), [entityId](const std::shared_ptr<IAiAction>& lhs, const std::shared_ptr<IAiAction>& rhs)
                {
                    const auto lhsApplicability = static_cast<int>(lhs->VGetApplicabilityForEntity(entityId));
                    const auto rhsApplicability = static_cast<int>(rhs->VGetApplicabilityForEntity(entityId));
                    
                    return lhsApplicability > rhsApplicability;
                });
                
                aiComponent.mCurrentAction = registeredActionsCopy.front();
                aiComponent.mCurrentAction->VStartForEntity(entityId);
            }
        }
        else
        {
            aiComponent.mBehaviourState = aiComponent.mCurrentAction->VGetActionBehaviourState();
            if (aiComponent.mCurrentAction->VUpdateForEntity(dt, entityId) == ActionStatus::FINISHED)
            {
                aiComponent.mCurrentAction = nullptr;
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

}
