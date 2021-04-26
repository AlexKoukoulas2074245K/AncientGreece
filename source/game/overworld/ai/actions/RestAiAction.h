///------------------------------------------------------------------------------------------------
///  RestAIAction.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef RestAiAction_h
#define RestAiAction_h

///-----------------------------------------------------------------------------------------------

#include "IAiAction.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------
class RestAiAction final : public IAiAction
{
public:
    RestAiAction();
    void VStartForEntity(const genesis::ecs::EntityId entityId) const override;
    ActionStatus VUpdateForEntity(const float dt, const genesis::ecs::EntityId entityId) const override;
    Applicability VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const override;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RestAiAction_h */
