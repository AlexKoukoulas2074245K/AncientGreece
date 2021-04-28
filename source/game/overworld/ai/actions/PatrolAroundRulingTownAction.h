///------------------------------------------------------------------------------------------------
///  PatrolAroundRulingTownAction.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 28/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef PatrolAroundRulingTownAction_h
#define PatrolAroundRulingTownAction_h

///-----------------------------------------------------------------------------------------------

#include "IAiAction.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------
class PatrolAroundRulingTownAction final : public IAiAction
{
public:
    PatrolAroundRulingTownAction();
    void VStartForEntity(const genesis::ecs::EntityId entityId) const override;
    ActionStatus VUpdateForEntity(const float dt, const genesis::ecs::EntityId entityId) const override;
    Applicability VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const override;
    BehaviourState VGetActionBehaviourState() const override;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* PatrolAroundRulingTownAction_h */
