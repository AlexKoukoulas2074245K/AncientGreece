///------------------------------------------------------------------------------------------------
///  LiveBattleStateSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 07/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef LiveBattleStateSingletonComponent_h
#define LiveBattleStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

enum class BattleState
{
    ONGOING, UNITS_CELEBRATING, FINISHED
};

///-----------------------------------------------------------------------------------------------

enum class BattleResult
{
    ATTACKER_VICTORIOUS, DEFENDER_VICTORIOUS
};

///-----------------------------------------------------------------------------------------------

class LiveBattleStateSingletonComponent final: public genesis::ecs::IComponent
{
public:
    BattleState mBattleState;
    BattleResult mBattleResult;
    float mCelebrationTimer;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* LiveBattleStateSingletonComponent_h */
