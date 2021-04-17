///------------------------------------------------------------------------------------------------
///  BattleStateSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 07/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleStateSingletonComponent_h
#define BattleStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

#include <tsl/robin_map.h>

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

class BattleStateSingletonComponent final: public genesis::ecs::IComponent
{
public:
    tsl::robin_map<StringId, tsl::robin_map<StringId, int, StringIdHasher>, StringIdHasher> mLeaderNameToCasualtiesMap;
    BattleState mBattleState;
    BattleResult mBattleResult;
    float mCelebrationTimer;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* BattleStateSingletonComponent_h */
