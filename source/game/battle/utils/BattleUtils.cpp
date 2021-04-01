///------------------------------------------------------------------------------------------------
///  BattleUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "BattleUtils.h"
#include "../components/BattleQueueSingletonComponent.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"

///------------------------------------------------------------------------------------------------

namespace battle
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const float BATTLE_CAMERA_X     = 0.02906f;
    static const float BATTLE_CAMERA_Y     = -0.28339f;
    static const float BATTLE_CAMERA_Z     = -0.24034f;
    static const float BATTLE_CAMERA_PITCH = 0.946990f;
    static const float BATTLE_CAMERA_YAW   = 1.6067f;
    static const float BATTLE_CAMERA_ROLL  = 0.6024f;
}

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

}

