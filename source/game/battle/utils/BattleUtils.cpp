///------------------------------------------------------------------------------------------------
///  BattleUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "BattleUtils.h"
#include "../components/BattleSideComponent.h"
#include "../components/BattleQueueSingletonComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitFactoryUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/MeshResource.h"

#include <unordered_map>
#include <algorithm>

///------------------------------------------------------------------------------------------------

namespace battle
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId BATTLE_GROUND_ENTITY_NAME = StringId("battle_grass");
    static const StringId BATTLE_UNITENTITY_NAME    = StringId("battle_unit");

    static const std::string BATTLE_GROUND_MODEL_NAME = "battle_grass";
    
    static const float BATTLE_DEFENDING_SIDE_Y_OFFSET = 0.2f;
    static const float BATTLE_CAMERA_X                = 0.02906f;
    static const float BATTLE_CAMERA_Y                = -0.28339f;
    static const float BATTLE_CAMERA_Z                = -0.24034f;
    static const float BATTLE_CAMERA_PITCH            = 0.946990f;
    static const float BATTLE_CAMERA_YAW              = 1.6067f;
    static const float BATTLE_CAMERA_ROLL             = 0.6024f;

    static const int MAX_UNITS_PER_ROW = 10;
}

///------------------------------------------------------------------------------------------------

void CreateBattleGround();
void CalculateUnitTargetDistances(const std::vector<UnitStats>& party, float& targetUnitXDistance, float& targetUnitYDistance);
void CreateBattleUnits(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const float targetUnitXDistance, const float targetUnitYDistance, const genesis::ecs::EntityId attackingLeaderEntityId, const genesis::ecs::EntityId defendingLeaderEntityId);
void CreateBattleUnitsOnSide(const std::vector<UnitStats>& sideParty, const float targetUnitXDistance, const float targetUnitYDistance, const StringId leaderEntityName, const bool isAttackingSide);

///------------------------------------------------------------------------------------------------

void QueueBattle(const genesis::ecs::EntityId attackingEntity, const genesis::ecs::EntityId defendingEntity)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<BattleQueueSingletonComponent>())
    {
        world.SetSingletonComponent<BattleQueueSingletonComponent>(std::make_unique<BattleQueueSingletonComponent>());
    }
    
    auto& battleQueueComponent = world.GetSingletonComponent<BattleQueueSingletonComponent>();
    battleQueueComponent.mQueuedBattles.push({attackingEntity, defendingEntity});
}

///-----------------------------------------------------------------------------------------------

void PopulateBattleEntities(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const genesis::ecs::EntityId attackingLeaderEntity, const genesis::ecs::EntityId defendingLeaderEntity)
{
    CreateBattleGround();
    
    float targetUnitXDistance, targetUnitYDistance;
    CalculateUnitTargetDistances(attackingSideParty, targetUnitXDistance, targetUnitYDistance);
    CreateBattleUnits(attackingSideParty, defendingSideParty, targetUnitXDistance, targetUnitYDistance, attackingLeaderEntity, defendingLeaderEntity);
}

///------------------------------------------------------------------------------------------------

void PrepareBattleCamera()
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    cameraComponent.mPosition.x = BATTLE_CAMERA_X;
    cameraComponent.mPosition.y = BATTLE_CAMERA_Y;
    cameraComponent.mPosition.z = BATTLE_CAMERA_Z;
    cameraComponent.mYaw        = BATTLE_CAMERA_YAW;
    cameraComponent.mPitch      = BATTLE_CAMERA_PITCH;
    cameraComponent.mRoll       = BATTLE_CAMERA_ROLL;
}

///-----------------------------------------------------------------------------------------------

void CreateBattleGround()
{
    genesis::rendering::LoadAndCreateStaticModelByName(BATTLE_GROUND_MODEL_NAME, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), BATTLE_GROUND_ENTITY_NAME);
}

///------------------------------------------------------------------------------------------------

void CalculateUnitTargetDistances(const std::vector<UnitStats>& party, float& targetUnitXDistance, float& targetUnitYDistance)
{
    auto& world = genesis::ecs::World::GetInstance();
    std::unordered_map<StringId, StringId, StringIdHasher> mUnitTypeSizeSeen;
    
    for (const auto& unitStats: party)
    {
        if (mUnitTypeSizeSeen.count(unitStats.mUnitType))
        {
            continue;
        }
        
        auto testUnitEntity = CreateUnit(unitStats.mUnitType, unitStats.mUnitName);
        
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(testUnitEntity);
        const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(testUnitEntity);
        const auto& meshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
        
        if (meshResource.GetDimensions().x * transformComponent.mScale.x > targetUnitXDistance)
        {
            targetUnitXDistance = meshResource.GetDimensions().x * transformComponent.mScale.x;
        }
        if (meshResource.GetDimensions().y * transformComponent.mScale.y > targetUnitYDistance)
        {
            targetUnitYDistance = meshResource.GetDimensions().y * transformComponent.mScale.y;
        }
        
        mUnitTypeSizeSeen[unitStats.mUnitType];
        world.DestroyEntity(testUnitEntity);
    }
}

///------------------------------------------------------------------------------------------------

void CreateBattleUnits(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const float targetUnitXDistance, const float targetUnitYDistance, const genesis::ecs::EntityId attackingLeaderEntityId, const genesis::ecs::EntityId defendingLeaderEntityId)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto attackingLeaderEntityName = world.GetComponent<genesis::NameComponent>(attackingLeaderEntityId).mName;
    const auto defendingLeaderEntityName = world.GetComponent<genesis::NameComponent>(defendingLeaderEntityId).mName;
    
    CreateBattleUnitsOnSide(attackingSideParty, targetUnitXDistance, targetUnitYDistance, attackingLeaderEntityName, true);
    CreateBattleUnitsOnSide(defendingSideParty, targetUnitXDistance, targetUnitYDistance, defendingLeaderEntityName, false);
}

///------------------------------------------------------------------------------------------------

void CreateBattleUnitsOnSide(const std::vector<UnitStats>& sideParty, const float targetUnitXDistance, const float targetUnitYDistance, const StringId leaderEntityName, const bool isAttackingSide)
{
    auto& world = genesis::ecs::World::GetInstance();
    
    auto xCounter = 0.0f;
    auto yCounter = isAttackingSide ? 0.0f : BATTLE_DEFENDING_SIDE_Y_OFFSET;
    auto unitCounter = 1;
    for (const auto& unitStats: sideParty)
    {
        auto unitEntityId = CreateUnit(unitStats.mUnitType, unitStats.mUnitName, BATTLE_UNITENTITY_NAME, glm::vec3(xCounter, yCounter, 0.0f), glm::vec3(0.0f, 0.0f, (isAttackingSide ? 0.0f : genesis::math::PI)));
        
        auto battleSideComponent = std::make_unique<BattleSideComponent>();
        battleSideComponent->mBattleSideLeaderName = leaderEntityName;
        world.AddComponent<BattleSideComponent>(unitEntityId, std::move(battleSideComponent));
        
        xCounter += targetUnitXDistance;
        
        if (++unitCounter > MAX_UNITS_PER_ROW)
        {
            unitCounter = 1;
            xCounter = 0;
            yCounter += isAttackingSide ? -targetUnitYDistance : targetUnitYDistance;
        }
    }
}

///------------------------------------------------------------------------------------------------

}

