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

#include <map>

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
    std::map<StringId, std::map<StringId, int, StringIdStdMapComparator>, StringIdStdMapComparator> mLeaderNameToCasualtiesMap;
    StringId mPlayerUnitName;
    StringId mAttackingLeaderUnitName;
    StringId mDefendingLeaderUnitName;
    StringId mAssistingAttackerLeaderUnitName;
    StringId mAssistingDefenderLeaderUnitName;
    BattleState mBattleState;
    BattleResult mBattleResult;
    float mCelebrationTimer;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* BattleStateSingletonComponent_h */
