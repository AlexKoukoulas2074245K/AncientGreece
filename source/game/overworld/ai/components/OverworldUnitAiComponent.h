///------------------------------------------------------------------------------------------------
///  OverworldUnitAiComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldUnitAiComponent_h
#define OverworldUnitAiComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../utils/OverworldDayTimeUtils.h"
#include "../../utils/TimeStamp.h"

#include <memory>
#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

class IAiAction;

///-----------------------------------------------------------------------------------------------

enum class BehaviourState
{
    RESTING,
    TRAVELLING,
    PATROLLING
};

///-----------------------------------------------------------------------------------------------

static const tsl::robin_map<BehaviourState, StringId> BEHAVIOUR_STATE_TO_STRING =
{
    { BehaviourState::RESTING, StringId("Resting") },
    { BehaviourState::TRAVELLING, StringId("Travelling") },
    { BehaviourState::PATROLLING, StringId("Patrolling") }
};

///-----------------------------------------------------------------------------------------------

class OverworldUnitAiComponent final: public genesis::ecs::IComponent
{
public:
    std::shared_ptr<IAiAction> mCurrentAction = nullptr;
    int mLastActionIndex = -1;
    TimeStamp mLastRestTimeStamp;
    BehaviourState mBehaviourState;
};

///-----------------------------------------------------------------------------------------------
 
}

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldUnitAiComponent_h */
