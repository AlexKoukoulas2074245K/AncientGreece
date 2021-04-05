///------------------------------------------------------------------------------------------------
///  BattleCollisionHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleCollisionHandlingSystem.h"
#include "../components/BattleUnitCollisionComponent.h"
#include "../components/BattleSideComponent.h"
#include "../../components/CollidableComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitCollisionUtils.h"
#include "../../scene/components/SceneStateSingletonComponent.h"
#include "../../scene/scenegraphs/ISceneGraph.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

BattleCollisionHandlingSystem::BattleCollisionHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleCollisionHandlingSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    tsl::robin_map<genesis::ecs::EntityId, glm::vec3> collisionDisplacements;
    BuildCollisionDisplacementMap(entitiesToProcess, collisionDisplacements);
    ApplyCollisionDisplacements(dt, collisionDisplacements);
}

///-----------------------------------------------------------------------------------------------

void BattleCollisionHandlingSystem::BuildCollisionDisplacementMap(const std::vector<genesis::ecs::EntityId>& entitiesToProcess, tsl::robin_map<genesis::ecs::EntityId, glm::vec3>& collisionDisplacementMap) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& sceneGraph = world.GetSingletonComponent<scene::SceneStateSingletonComponent>().mSceneGraph;
    
    for (const auto entityId: entitiesToProcess)
    {
        if (!world.HasEntity(entityId))
        {
            continue;
        }
        
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        const auto& collisionCandidates = sceneGraph->VGetCollisionCandidates(entityId);
        for (const auto& candidateEntity: collisionCandidates)
        {
            if (!world.HasEntity(candidateEntity))
            {
                continue;
            }
            
            float penetration;
            const auto entitiesColliding = AreEntitiesColliding(entityId, candidateEntity, penetration);
            
            if (entitiesColliding)
            {
                const auto& otherTransformComponent = world.GetComponent<genesis::TransformComponent>(candidateEntity);
                
                // Calculate push direction from other entity to current entity
                auto directionToCurrentEntity = glm::normalize(transformComponent.mPosition - otherTransformComponent.mPosition);
                
                // Rotate direction vector by a random angle to avoid entities stacking in a queue
                directionToCurrentEntity = glm::rotateZ(directionToCurrentEntity, genesis::math::RandomFloat(-0.2f, 0.2f));
                
                // Add half penetration displacement to both entities
                collisionDisplacementMap[entityId] += directionToCurrentEntity * penetration * 0.5f;
                collisionDisplacementMap[candidateEntity] -= directionToCurrentEntity * penetration * 0.5f;
            }
        }
    }
    
}

///-----------------------------------------------------------------------------------------------

void BattleCollisionHandlingSystem::ApplyCollisionDisplacements(const float dt, const tsl::robin_map<genesis::ecs::EntityId, glm::vec3>& collisionDisplacementMap) const
{
    auto& world = genesis::ecs::World::GetInstance();
    for (const auto& pair: collisionDisplacementMap)
    {
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(pair.first);
        transformComponent.mPosition += pair.second * dt;
    }
}

///-----------------------------------------------------------------------------------------------

}
