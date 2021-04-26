///------------------------------------------------------------------------------------------------
///  IAiAction.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef IAiAction_h
#define IAiAction_h

///-----------------------------------------------------------------------------------------------

#include "../../../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

enum class Applicability
{
    NOT_APPLICABLE       = 0,
    LOW_APPLICABILITY    = 1,
    MEDIUM_APPLICABILITY = 2,
    HIGH_APPLICABILITY   = 3
};

///-----------------------------------------------------------------------------------------------

enum class ActionStatus
{
    ONGOING,
    FINISHED
};

///-----------------------------------------------------------------------------------------------

class IAiAction
{
public:
    virtual ~IAiAction() = default;
    virtual void VStartForEntity(const genesis::ecs::EntityId entityId) const = 0;
    virtual ActionStatus VUpdateForEntity(const float dt, const genesis::ecs::EntityId entityId) const = 0;
    virtual Applicability VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const = 0;
    
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* IAiAction_h */
