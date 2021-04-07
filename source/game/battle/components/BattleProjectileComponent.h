///------------------------------------------------------------------------------------------------
///  BattleProjectileComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 06/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleProjectileComponent_h
#define BattleProjectileComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

enum class ProjectileState
{
    SEEKING_TARGET,
    ATTACHED_TO_TARGET
};

class BattleProjectileComponent final: public genesis::ecs::IComponent
{
public:
    glm::vec3 mTargetAttachmentDelta;
    glm::vec3 mOffsetTargetPosition;
    int mDamage;
    ProjectileState mState;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* BattleProjectileComponent_h */
