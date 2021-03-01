///------------------------------------------------------------------------------------------------
///  ECS.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "ECS.h"
#include "common/components/NameComponent.h"

#include <chrono>
#include <typeinfo>

///------------------------------------------------------------------------------------------------

namespace genesis
{ 

///------------------------------------------------------------------------------------------------

namespace ecs
{ 

///------------------------------------------------------------------------------------------------

#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
static StringId GetSystemNameFromTypeIdString(const std::string& typeIdString)
{
    const auto& systemNameSplitByWhiteSpace = StringSplit(typeIdString, ' ');
    const auto& systemNameSplitByColumn = StringSplit(systemNameSplitByWhiteSpace[systemNameSplitByWhiteSpace.size() - 1], ':');
    return StringId(systemNameSplitByColumn[systemNameSplitByColumn.size() - 1]);
}
#endif

///------------------------------------------------------------------------------------------------

World& World::GetInstance()
{
    static World instance;
    return instance;
}

///------------------------------------------------------------------------------------------------

const tsl::robin_map<StringId, long long, StringIdHasher>& World::GetSystemUpdateTimes() const
{
    return mSystemUpdateToDuration;
}


///------------------------------------------------------------------------------------------------

void World::AddSystem(std::unique_ptr<ISystem> system)
{
    auto& systemRef = *system;
    
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    system->mSystemName = GetSystemNameFromTypeIdString(std::string(typeid(systemRef).name()));
#endif

    mSystems.push_back(std::move(system));
    mEntitiesToUpdatePerSystem[typeid(systemRef)];
}

///------------------------------------------------------------------------------------------------

void World::Update(const float dt)
{
    RemoveEntitiesWithoutAnyComponents();
    
    for(const auto& system: mSystems)
    {     
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
        const auto& start = std::chrono::high_resolution_clock::now();
#endif
        auto& systemRef = *system;
        const auto& entityVec = mEntitiesToUpdatePerSystem.at(typeid(systemRef));
        
        system->VUpdate(dt, entityVec);

#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)        
        const auto& end = std::chrono::high_resolution_clock::now();
        const auto& duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        mSystemUpdateToDuration[StringId(system->mSystemName)] = duration.count();
#endif
    }
}

///------------------------------------------------------------------------------------------------

EntityId World::CreateEntity()
{
    mEntityComponentStore.operator[](mEntityCounter);
    return mEntityCounter++;
}

///------------------------------------------------------------------------------------------------

EntityId World::CreateEntity(const StringId& name)
{
    const auto entity = CreateEntity();    
    AddComponent<NameComponent>(entity, std::make_unique<NameComponent>(name));
    return entity;
}

///------------------------------------------------------------------------------------------------

bool World::HasEntity(const EntityId entityId) const
{
    return mEntityComponentStore.count(entityId) != 0;
}

///------------------------------------------------------------------------------------------------

void World::DestroyEntity(const EntityId entityId)
{
    assert(entityId != NULL_ENTITY_ID &&
        "NULL_ENTITY_ID entity removal request");

    assert(mEntityComponentStore.count(entityId) != 0 &&
        "Entity does not exist in the world");

    mEntityComponentStore.at(entityId).mMask.reset();
    OnEntityChanged(entityId, ComponentMask());
}

///------------------------------------------------------------------------------------------------

void World::DestroyEntities(const std::vector<EntityId>& entityIds)
{
    std::for_each(entityIds.cbegin(), entityIds.cend(), [&](const EntityId& entityId)
    {
        DestroyEntity(entityId);
    });
}

///------------------------------------------------------------------------------------------------

EntityId World::FindEntityWithName(const StringId& entityName) const
{
    for (const auto& entityComponentStoreEntry: mEntityComponentStore)
    {
        const auto& entityId = entityComponentStoreEntry.first;
        if (HasComponent<NameComponent>(entityId) && GetComponent<NameComponent>(entityId).mName == entityName)
        {
            return entityId;
        }
    }
    
    return ecs::NULL_ENTITY_ID;
}

///------------------------------------------------------------------------------------------------

std::vector<EntityId> World::FindAllEntitiesWithName(const StringId &entityName) const
{
    std::vector<EntityId> mEntityIds;
    
    for (const auto& entityComponentStoreEntry: mEntityComponentStore)
    {
        const auto& entityId = entityComponentStoreEntry.first;
        if (HasComponent<NameComponent>(entityId) && GetComponent<NameComponent>(entityId).mName == entityName)
        {
            mEntityIds.push_back(entityId);
        }
    }
    
    return mEntityIds;
}

///------------------------------------------------------------------------------------------------

std::size_t World::GetEntityCount() const
{
    return mEntityComponentStore.size();
}

///------------------------------------------------------------------------------------------------

void World::RemoveEntitiesWithoutAnyComponents()
{
    auto entityIter = mEntityComponentStore.begin();
    while (entityIter != mEntityComponentStore.end())
    {                
        if (entityIter->second.mMask.any() == false)
        {
            entityIter = mEntityComponentStore.erase(entityIter);
        }
        else
        {
            entityIter++;
        }
    }
}

///------------------------------------------------------------------------------------------------

void World::OnEntityChanged(const EntityId entityId, const ComponentMask& newComponentMask)
{
    for (auto& system: mSystems)
    {
        auto& systemRef = *system;
        auto& systemEntityVec = mEntitiesToUpdatePerSystem[typeid(systemRef)];
        
        if
        (
            system->ShouldProcessComponentMask(newComponentMask) &&
            std::find(systemEntityVec.cbegin(), systemEntityVec.cend(), entityId) == systemEntityVec.cend()
        )
        {
            systemEntityVec.push_back(entityId);
        }
        else if
        (
            system->ShouldProcessComponentMask(newComponentMask) == false &&
            std::find(systemEntityVec.cbegin(), systemEntityVec.cend(), entityId) != systemEntityVec.cend()
        )
        {
            systemEntityVec.erase(std::remove(systemEntityVec.begin(), systemEntityVec.end(), entityId), systemEntityVec.end());
        }
    }
}

///------------------------------------------------------------------------------------------------

World::World()
{
    mEntityComponentStore.reserve(ANTICIPATED_ENTITY_COUNT);
}

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------
