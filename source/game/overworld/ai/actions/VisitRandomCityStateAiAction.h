///------------------------------------------------------------------------------------------------
///  VisitRandomCityStateAiAction.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef VisitRandomCityStateAiAction_h
#define VisitRandomCityStateAiAction_h

///-----------------------------------------------------------------------------------------------

#include "IAiAction.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------
class VisitRandomCityStateAiAction final : public IAiAction
{
public:
    void VStartForEntity(const genesis::ecs::EntityId entityId) const override;
    ActionStatus VUpdateForEntity(const float dt, const genesis::ecs::EntityId entityId) const override;
    Applicability VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const override;
    BehaviourState VGetActionBehaviourState() const override;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* VisitRandomCityStateAiAction_h */
