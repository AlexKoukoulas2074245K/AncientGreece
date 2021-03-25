///------------------------------------------------------------------------------------------------
///  KeyValueUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 24/03/2021.
///------------------------------------------------------------------------------------------------

#include "KeyValueUtils.h"
#include "../components/KeyValueDataStoreSingletonComponent.h"

///------------------------------------------------------------------------------------------------

namespace
{
    static const std::string MISSING_VALUE_STRING = "MISSING VALUE ";
}

///------------------------------------------------------------------------------------------------

void WriteValue(const StringId key, const std::string& value)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<KeyValueDataStoreSingletonComponent>())
    {
        world.SetSingletonComponent<KeyValueDataStoreSingletonComponent>(std::make_unique<KeyValueDataStoreSingletonComponent>());
    }
    world.GetSingletonComponent<KeyValueDataStoreSingletonComponent>().mKeyValueStore[key] = value;
}

///------------------------------------------------------------------------------------------------

const std::string ReadValue(const StringId key)
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasSingletonComponent<KeyValueDataStoreSingletonComponent>())
    {
        world.SetSingletonComponent<KeyValueDataStoreSingletonComponent>(std::make_unique<KeyValueDataStoreSingletonComponent>());
    }
    
    auto& keyValueComponent = world.GetSingletonComponent<KeyValueDataStoreSingletonComponent>();
    
    return keyValueComponent.mKeyValueStore.count(key) ? keyValueComponent.mKeyValueStore.at(key) : MISSING_VALUE_STRING + key.GetString();
}

///-----------------------------------------------------------------------------------------------


