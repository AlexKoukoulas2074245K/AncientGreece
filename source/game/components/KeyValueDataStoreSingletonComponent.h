///------------------------------------------------------------------------------------------------
///  KeyValueDataStoreSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 24/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef KeyValueDataStoreSingletonComponent_h
#define KeyValueDataStoreSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

class KeyValueDataStoreSingletonComponent final: public genesis::ecs::IComponent
{
public:
    tsl::robin_map<StringId, std::string, StringIdHasher> mKeyValueStore;
};

///-----------------------------------------------------------------------------------------------

#endif /* KeyValueDataStoreSingletonComponent_h */
