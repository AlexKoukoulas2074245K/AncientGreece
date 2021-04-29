///------------------------------------------------------------------------------------------------
///  SeekUnitFightAiAction.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef SeekUnitFightAiAction_h
#define SeekUnitFightAiAction_h

///-----------------------------------------------------------------------------------------------

#include "IAiAction.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------
class SeekUnitFightAiAction final : public IAiAction
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

#endif /* SeekUnitFightAiAction_h */
