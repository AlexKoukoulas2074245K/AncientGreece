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
#include "../UnitStats.h"
#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

class UnitBaseStatsSingletonComponent final: public genesis::ecs::IComponent
{
public:
    tsl::robin_map<StringId, UnitStats, StringIdHasher> mUnitTypeNameToBaseStats;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitBaseStatsSingletonComponent_h */
