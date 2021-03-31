///------------------------------------------------------------------------------------------------
///  UnitBaseStatsSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef UnitBaseStatsSingletonComponent_h
#define UnitBaseStatsSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

struct UnitBaseStats
{
    StringId mUnitModelName = StringId();
    float mUnitModelScaleFactor = 1.0f;
    float mAttackAnimationDamageTrigger = 1.0f;
    int mBaseDamage = 0;
};

///-----------------------------------------------------------------------------------------------

class UnitBaseStatsSingletonComponent final: public genesis::ecs::IComponent
{
public:
    tsl::robin_map<StringId, UnitBaseStats, StringIdHasher> mUnitTypeNameToBaseStats;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitBaseStatsSingletonComponent_h */
