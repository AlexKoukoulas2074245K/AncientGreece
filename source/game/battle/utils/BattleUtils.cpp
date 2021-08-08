///------------------------------------------------------------------------------------------------
///  BattleUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "BattleUtils.h"
#include "../components/BattleDamageComponent.h"
#include "../components/BattleSideComponent.h"
#include "../../components/CollidableComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitFactoryUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"
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
    static const StringId BATTLE_MAP_ENTITY_NAME        = StringId("battle_grass");
    static const StringId BATTLE_UNIT_ENTITY_NAME       = StringId("battle_unit");
    static const StringId BATTLE_PROJECTILE_ENTITY_NAME = StringId("battle_projectile");
    
    static const std::string BATTLE_HEIGHT_MAP_NAME = "battle";

    static const float BATTLE_DEFENDING_SIDE_Y_OFFSET     = 0.2f;
    static const float BATTLE_CAMERA_X                    = 0.02906f;
    static const float BATTLE_CAMERA_Y                    = -0.28339f;
    static const float BATTLE_CAMERA_Z                    = -0.24034f;
    static const float BATTLE_CAMERA_PITCH                = 0.946990f;
    static const float BATTLE_CAMERA_YAW                  = 1.6067f;
    static const float BATTLE_CAMERA_ROLL                 = 0.6024f;
    static const float MELEE_COLLISION_UPSCALE_FACTOR     = 1.2f;
    static const float ENTITY_SPHERE_COLLISION_MULTIPLIER = 0.25f * 0.3333f;

    static const int MAX_UNITS_PER_ROW = 10;
}

///------------------------------------------------------------------------------------------------

void CreateBattleGround();
void CalculateUnitTargetDistances(const std::vector<UnitStats>& party, float& targetUnitXDistance, float& targetUnitYDistance);
void CreateBattleUnits(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const std::vector<UnitStats>& assistingAttackerParty, const std::vector<UnitStats>& assistingDefenderParty, const float targetUnitXDistance, const float targetUnitYDistance, const genesis::ecs::EntityId attackingLeaderEntityId, const genesis::ecs::EntityId defendingLeaderEntityId, const genesis::ecs::EntityId assistingAttackerLeaderEntity, const genesis::ecs::EntityId assistingDefenderLeaderEntity);
void CreateBattleUnitsOnSide(const std::vector<UnitStats>& sideParty, const float targetUnitXDistance, const float targetUnitYDistance, const StringId& leaderUnitName, const StringId& assistingLeaderUnitName, const bool isAttackingSide, const bool isAssistingParty, const bool hasAssistingParty);

std::vector<UnitStats> FindLargestBattleParty(const std::vector<UnitStats>& firstParty, const std::vector<UnitStats>& secondParty, const std::vector<UnitStats>& thirdParty, const std::vector<UnitStats>& fourthParty);

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetMapEntity()
{
    return genesis::ecs::World::GetInstance().FindEntityWithName(BATTLE_MAP_ENTITY_NAME);
}

///------------------------------------------------------------------------------------------------

StringId GetProjectileEntityName()
{
    return BATTLE_PROJECTILE_ENTITY_NAME;
}

///------------------------------------------------------------------------------------------------

std::vector<genesis::ecs::EntityId> GetAllBattleUnitEntities()
{
    return genesis::ecs::World::GetInstance().FindAllEntitiesWithName(BATTLE_UNIT_ENTITY_NAME);
}

///------------------------------------------------------------------------------------------------

bool IsBattleFinished()
{
    return  genesis::ecs::World::GetInstance().GetSingletonComponent<BattleStateSingletonComponent>().mBattleState != BattleState::ONGOING;
}

///------------------------------------------------------------------------------------------------

void SetBattleState(const BattleState battleState)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<BattleStateSingletonComponent>())
    {
        world.SetSingletonComponent<BattleStateSingletonComponent>(std::make_unique<BattleStateSingletonComponent>());
    }
    
    world.GetSingletonComponent<BattleStateSingletonComponent>().mBattleState = battleState;
}

///------------------------------------------------------------------------------------------------

void SetBattleLeaderNames(const StringId& attackingLeaderUnitName, const StringId& defendingLeaderUnitName, const StringId& assistingAttackerLeaderUnitName, const StringId& assistingDefenderLeaderUnitName, const StringId& playerUnitName)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<BattleStateSingletonComponent>())
    {
        world.SetSingletonComponent<BattleStateSingletonComponent>(std::make_unique<BattleStateSingletonComponent>());
    }
    
    auto& battleStateComponent = world.GetSingletonComponent<BattleStateSingletonComponent>();
    battleStateComponent.mAttackingLeaderUnitName         = attackingLeaderUnitName;
    battleStateComponent.mDefendingLeaderUnitName         = defendingLeaderUnitName;
    battleStateComponent.mAssistingAttackerLeaderUnitName = assistingAttackerLeaderUnitName;
    battleStateComponent.mAssistingDefenderLeaderUnitName = assistingDefenderLeaderUnitName;
    battleStateComponent.mPlayerUnitName                  = playerUnitName;
}

///------------------------------------------------------------------------------------------------

void InitCasualties(const StringId& attackingLeaderUnitName, const StringId& defendingLeaderUnitName, const StringId& assistingAttackerLeaderUnitName, const StringId& assistingDefenderLeaderUnitName)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& battleStateComponent = world.GetSingletonComponent<BattleStateSingletonComponent>();
    
    battleStateComponent.mLeaderNameToCasualtiesMap[attackingLeaderUnitName];
    battleStateComponent.mLeaderNameToCasualtiesMap[defendingLeaderUnitName];
    
    if (assistingAttackerLeaderUnitName != StringId())
    {
        battleStateComponent.mLeaderNameToCasualtiesMap[assistingAttackerLeaderUnitName];
    }
    if (assistingDefenderLeaderUnitName != StringId())
    {
        battleStateComponent.mLeaderNameToCasualtiesMap[assistingDefenderLeaderUnitName];
    }
}

///------------------------------------------------------------------------------------------------

void DamageUnit(const genesis::ecs::EntityId unitEntity, const int damage)
{
    if (IsUnitDead(unitEntity))
    {
        return;
    }
    
    auto& world = genesis::ecs::World::GetInstance();
    if (world.HasComponent<BattleDamageComponent>(unitEntity))
    {
        world.GetComponent<BattleDamageComponent>(unitEntity).mDamage += damage;
    }
    else
    {
        auto damageComponent = std::make_unique<BattleDamageComponent>();
        damageComponent->mDamage = damage;
        world.AddComponent<BattleDamageComponent>(unitEntity, std::move(damageComponent));
    }
}

///------------------------------------------------------------------------------------------------

void AddBattleCasualty(const StringId unitType, const StringId leaderName)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& battleStateComponent = world.GetSingletonComponent<BattleStateSingletonComponent>();
    
    if (battleStateComponent.mLeaderNameToCasualtiesMap.at(leaderName).count(unitType) == 0)
    {
        battleStateComponent.mLeaderNameToCasualtiesMap.at(leaderName)[unitType] = 1;
    }
    else
    {
        battleStateComponent.mLeaderNameToCasualtiesMap.at(leaderName)[unitType]++;
    }
}

///------------------------------------------------------------------------------------------------

void AddCollidableDataToArrow(const genesis::ecs::EntityId arrowEntity)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& entityTransformComponent =
    world.GetComponent<genesis::TransformComponent>(arrowEntity);
    const auto& entityRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(arrowEntity);
    const auto& entityMeshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( entityRenderableComponent.mMeshResourceIds[entityRenderableComponent.mCurrentMeshResourceIndex]);
    const auto entityScaledDimensions = entityMeshResource.GetDimensions() * entityTransformComponent.mScale;
    const auto entitySphereRadius = (entityScaledDimensions.x + entityScaledDimensions.y + entityScaledDimensions.z) * ENTITY_SPHERE_COLLISION_MULTIPLIER;
    
    auto collidableComponent = std::make_unique<CollidableComponent>();
    collidableComponent->mCollidableDimensions.x = collidableComponent->mCollidableDimensions.y = collidableComponent->mCollidableDimensions.z = entitySphereRadius;
    
    world.AddComponent<CollidableComponent>(arrowEntity, std::move(collidableComponent));
}

///-----------------------------------------------------------------------------------------------

void PopulateBattleEntities(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const std::vector<UnitStats>& assistingAttackerParty, const std::vector<UnitStats>& assistingDefenderParty, const genesis::ecs::EntityId attackingLeaderEntity, const genesis::ecs::EntityId defendingLeaderEntity, const genesis::ecs::EntityId assistingAttackerLeaderEntity, const genesis::ecs::EntityId assistingDefenderLeaderEntity)
{
    CreateBattleGround();
    
    float targetUnitXDistance = 0.0f;
    float targetUnitYDistance = 0.0f;
    
    const auto& largestParty = FindLargestBattleParty(attackingSideParty, defendingSideParty, assistingAttackerParty, assistingDefenderParty);
    
    CalculateUnitTargetDistances(largestParty, targetUnitXDistance, targetUnitYDistance);
    CreateBattleUnits(attackingSideParty, defendingSideParty, assistingAttackerParty, assistingDefenderParty, targetUnitXDistance, targetUnitYDistance, attackingLeaderEntity, defendingLeaderEntity, assistingAttackerLeaderEntity, assistingDefenderLeaderEntity);
}

///------------------------------------------------------------------------------------------------

void DestroyBattleEntities()
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto mapEntity = GetMapEntity();
    
    auto& resourceLoadingService = genesis::resources::ResourceLoadingService::GetInstance();
    auto& mapRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(mapEntity);
    resourceLoadingService.UnloadResource(mapRenderableComponent.mTextureResourceId);
    world.DestroyEntity(world.FindEntityWithName(BATTLE_MAP_ENTITY_NAME));
    world.DestroyEntities(world.FindAllEntitiesWithName(BATTLE_UNIT_ENTITY_NAME));
    world.DestroyEntities(world.FindAllEntitiesWithName(BATTLE_PROJECTILE_ENTITY_NAME));
}

///------------------------------------------------------------------------------------------------

void PrepareBattleCamera(const bool isPlayerDefending)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    cameraComponent.mPosition.x  = BATTLE_CAMERA_X;
    cameraComponent.mPosition.y  = BATTLE_CAMERA_Y;
    cameraComponent.mPosition.z  = BATTLE_CAMERA_Z;
    cameraComponent.mYaw         = BATTLE_CAMERA_YAW;
    cameraComponent.mPitch       = BATTLE_CAMERA_PITCH;
    cameraComponent.mRoll        = BATTLE_CAMERA_ROLL;
    
    if (isPlayerDefending)
    {
        cameraComponent.mRoll += genesis::math::PI;
    }
    
    cameraComponent.mCameraState = genesis::rendering::CameraState::AUTO_CENTERING;
}

///-----------------------------------------------------------------------------------------------

bool AreUnitsInMeleeDistance(const genesis::ecs::EntityId unitEntityA, const genesis::ecs::EntityId unitEntityB)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto& unitATransformComponent = world.GetComponent<genesis::TransformComponent>(unitEntityA);
    const auto& unitBTransformComponent = world.GetComponent<genesis::TransformComponent>(unitEntityB);
    
    const auto& unitACollidableComponent = world.GetComponent<CollidableComponent>(unitEntityA);
    const auto& unitBCollidableComponent = world.GetComponent<CollidableComponent>(unitEntityB);
    
    return genesis::math::SphereToSphereIntersection(unitATransformComponent.mPosition, unitACollidableComponent.mCollidableDimensions.x * MELEE_COLLISION_UPSCALE_FACTOR, unitBTransformComponent.mPosition, unitBCollidableComponent.mCollidableDimensions.x * MELEE_COLLISION_UPSCALE_FACTOR);
}

///-----------------------------------------------------------------------------------------------

bool AreUnitsInDoubleMeleeDistance(const genesis::ecs::EntityId unitEntityA, const genesis::ecs::EntityId unitEntityB)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto& unitATransformComponent = world.GetComponent<genesis::TransformComponent>(unitEntityA);
    const auto& unitBTransformComponent = world.GetComponent<genesis::TransformComponent>(unitEntityB);
    
    const auto& unitACollidableComponent = world.GetComponent<CollidableComponent>(unitEntityA);
    const auto& unitBCollidableComponent = world.GetComponent<CollidableComponent>(unitEntityB);
    
    return genesis::math::SphereToSphereIntersection(unitATransformComponent.mPosition, unitACollidableComponent.mCollidableDimensions.x * MELEE_COLLISION_UPSCALE_FACTOR * 2.0f, unitBTransformComponent.mPosition, unitBCollidableComponent.mCollidableDimensions.x * MELEE_COLLISION_UPSCALE_FACTOR * 2.0f);
}

///-----------------------------------------------------------------------------------------------

void CreateBattleGround()
{
    genesis::rendering::LoadAndCreateHeightMapByName(BATTLE_HEIGHT_MAP_NAME, 0.05f, 4.0f, BATTLE_MAP_ENTITY_NAME, genesis::rendering::HeightMapGenerationType::RANDOM_LOW_ROUGHNESS);
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

void CreateBattleUnits(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const std::vector<UnitStats>& assistingAttackerParty, const std::vector<UnitStats>& assistingDefenderParty, const float targetUnitXDistance, const float targetUnitYDistance, const genesis::ecs::EntityId attackingLeaderEntityId, const genesis::ecs::EntityId defendingLeaderEntityId, const genesis::ecs::EntityId assistingAttackerLeaderEntity, const genesis::ecs::EntityId assistingDefenderLeaderEntity)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto attackingLeaderUnitName = world.GetComponent<UnitStatsComponent>(attackingLeaderEntityId).mStats.mUnitName;
    const auto defendingLeaderUnitName = world.GetComponent<UnitStatsComponent>(defendingLeaderEntityId).mStats.mUnitName;
    
    CreateBattleUnitsOnSide(attackingSideParty, targetUnitXDistance, targetUnitYDistance, attackingLeaderUnitName, StringId(), true, false, assistingAttackerLeaderEntity != genesis::ecs::NULL_ENTITY_ID);
    CreateBattleUnitsOnSide(defendingSideParty, targetUnitXDistance, targetUnitYDistance, defendingLeaderUnitName, StringId(), false, false, assistingDefenderLeaderEntity != genesis::ecs::NULL_ENTITY_ID);
    
    if (assistingAttackerLeaderEntity != genesis::ecs::NULL_ENTITY_ID)
    {
        const auto assistingLeaderUnitName = world.GetComponent<UnitStatsComponent>(assistingAttackerLeaderEntity).mStats.mUnitName;
        
        CreateBattleUnitsOnSide(assistingAttackerParty, targetUnitXDistance, targetUnitYDistance, attackingLeaderUnitName, assistingLeaderUnitName, true, true, false);
    }
    if (assistingDefenderLeaderEntity != genesis::ecs::NULL_ENTITY_ID)
    {
        const auto assistingLeaderUnitName = world.GetComponent<UnitStatsComponent>(assistingDefenderLeaderEntity).mStats.mUnitName;
        CreateBattleUnitsOnSide(assistingDefenderParty, targetUnitXDistance, targetUnitYDistance, defendingLeaderUnitName, assistingLeaderUnitName, false, true, false);
    }
}

///------------------------------------------------------------------------------------------------

void CreateBattleUnitsOnSide(const std::vector<UnitStats>& sideParty, const float targetUnitXDistance, const float targetUnitYDistance, const StringId& leaderUnitName, const StringId& assistingLeaderUnitName, const bool isAttackingSide, const bool isAssistingParty, const bool hasAssistingParty)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto groundEntity = GetMapEntity();
    
    auto xCounter = 0.0f;
    if (isAssistingParty) xCounter += MAX_UNITS_PER_ROW * targetUnitXDistance;
    if (hasAssistingParty) xCounter -= MAX_UNITS_PER_ROW * targetUnitXDistance;
    
    auto yCounter = isAttackingSide ? 0.0f : BATTLE_DEFENDING_SIDE_Y_OFFSET;
    auto unitCounter = 1;
    for (const auto& unitStats: sideParty)
    {
        auto targetZ = genesis::rendering::GetTerrainHeightAtPosition(groundEntity, glm::vec3(xCounter, yCounter, 0.0f));
        auto unitEntityId = CreateUnit(unitStats.mUnitType, unitStats.mUnitName, BATTLE_UNIT_ENTITY_NAME, glm::vec3(xCounter, yCounter, targetZ), glm::vec3(0.0f, 0.0f, (isAttackingSide ? 0.0f : genesis::math::PI)));
        
        auto battleSideComponent = std::make_unique<BattleSideComponent>();
        battleSideComponent->mBattleSideLeaderUnitName = leaderUnitName;
        battleSideComponent->mBattleSideAssistingLeaderUnitName = assistingLeaderUnitName;
        world.AddComponent<BattleSideComponent>(unitEntityId, std::move(battleSideComponent));
        
        xCounter += targetUnitXDistance;
        
        if (++unitCounter > MAX_UNITS_PER_ROW)
        {
            unitCounter = 1;
            xCounter = 0;
            if (isAssistingParty) xCounter += MAX_UNITS_PER_ROW * targetUnitXDistance;
            if (hasAssistingParty) xCounter -= MAX_UNITS_PER_ROW * targetUnitXDistance;
            yCounter += isAttackingSide ? -targetUnitYDistance : targetUnitYDistance;
        }
    }
}

///------------------------------------------------------------------------------------------------

std::vector<UnitStats> FindLargestBattleParty(const std::vector<UnitStats>& firstParty, const std::vector<UnitStats>& secondParty, const std::vector<UnitStats>& thirdParty, const std::vector<UnitStats>& fourthParty)
{
    std::vector<std::vector<UnitStats>> partyCollection = { firstParty, secondParty, thirdParty, fourthParty };
    std::sort(partyCollection.begin(), partyCollection.end(), [](const std::vector<UnitStats>& lhs, const std::vector<UnitStats>& rhs)
    {
        return lhs.size() > rhs.size();
    });
    return partyCollection.front();
}



///------------------------------------------------------------------------------------------------

}

