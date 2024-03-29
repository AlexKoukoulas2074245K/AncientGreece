///------------------------------------------------------------------------------------------------
///  ECS.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "ECS.h"
#include "common/components/NameComponent.h"

#include <chrono>
#include <thread>
#include <typeinfo>

///------------------------------------------------------------------------------------------------

namespace genesis
{ 

///------------------------------------------------------------------------------------------------

namespace ecs
{ 

///------------------------------------------------------------------------------------------------

class SystemUpdateWorker
{
public:
    void StartWorker(const std::function<void()>& onCompleteCallback)
    {
        mOnCompleteCallback = onCompleteCallback;
        mThread = std::thread([&]
        {
            while(true)
            {
                if (mCurrentProcessingVec.size() > 0 && mHasAssignedWorkParams)
                {
                    mSystem->VUpdate(mCurrentDt, mCurrentProcessingVec);
                    mCurrentProcessingVec.clear();
                    mHasAssignedWorkParams = false;
                    mOnCompleteCallback();
                }
            }
        });
        mThread.detach();
    }

    void AssignWorkParams
    (
        const float dt, 
        const std::vector<EntityId> entityVec,
        ISystem* system
    )
    {
        mCurrentProcessingVec = std::move(entityVec);
        mCurrentDt = dt;
        mSystem = system;
        mHasAssignedWorkParams = true;
    }
    
private:
    std::vector<genesis::ecs::EntityId> mCurrentProcessingVec;
    std::thread mThread;
    std::function<void()> mOnCompleteCallback;
    ISystem* mSystem;
    float mCurrentDt;
    bool mHasAssignedWorkParams = false;
};

///------------------------------------------------------------------------------------------------

#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
static StringId GetSystemNameFromTypeIdString(const std::string& typeIdString)
{
    std::string unsymbolifiedName;
    for (int i = typeIdString.size() - 1; i >= 0; i--)
    {
        if (std::isdigit(typeIdString[i])) break;
        unsymbolifiedName = typeIdString[i] + unsymbolifiedName;
    }
    
    const auto& systemNameSplitByWhiteSpace = StringSplit(unsymbolifiedName, ' ');
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

void World::AddSystem(std::unique_ptr<ISystem> system, const int contextIdToOperateIn /* 0 */, SystemOperationMode operationMode /* SystemOperationMode::SINGLE_THREADED */)
{
    auto& systemRef = *system;
    
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    system->mSystemName = GetSystemNameFromTypeIdString(std::string(typeid(systemRef).name()));
#endif
    
    if (operationMode == SystemOperationMode::MULTI_THREADED && mSystemUpdateWorkers.size() > 0)
    {
        system->mMultithreadedOperation = true;
    }
    system->mContextIdToOperateIn = contextIdToOperateIn;
    
    mSystems.push_back(std::move(system));
    mEntitiesToUpdatePerSystem[typeid(systemRef)];
}

///------------------------------------------------------------------------------------------------

void World::ChangeContext(const int contextId)
{
    mCurrentContextId = contextId;
}

///------------------------------------------------------------------------------------------------

int World::GetContext() const
{
    return mCurrentContextId;
}

///------------------------------------------------------------------------------------------------

void World::Update(const float dt)
{
    RemoveEntitiesWithoutAnyComponents();
    
    for(const auto& system: mSystems)
    {     
        if (system->mContextIdToOperateIn == 0 || mCurrentContextId == system->mContextIdToOperateIn)
        {
            auto& systemRef = *system;
            const auto& entityVec = mEntitiesToUpdatePerSystem.at(typeid(systemRef));
            
            const auto systemUpdateWorkerCount = mSystemUpdateWorkers.size();
            if (system->mMultithreadedOperation && systemUpdateWorkerCount > 0 && entityVec.size() >= systemUpdateWorkerCount)
            {
                const auto entityVecSlice = static_cast<int>(entityVec.size()/systemUpdateWorkerCount);
                for (auto i = 0U; i < systemUpdateWorkerCount - 1; ++i)
                {
                    const auto startSlice = i * entityVecSlice;
                    const auto endSlice = (i + 1) * entityVecSlice;
                    mSystemUpdateWorkers[i]->AssignWorkParams(dt, std::vector<ecs::EntityId>(entityVec.cbegin() + startSlice, entityVec.cbegin() + endSlice), system.get());
                }
                const auto startSlice = (systemUpdateWorkerCount - 1) * entityVecSlice;
                mSystemUpdateWorkers[systemUpdateWorkerCount - 1]->AssignWorkParams(dt, std::vector<ecs::EntityId>(entityVec.cbegin() + startSlice, entityVec.end()), system.get());

                while (mSystemUpdateWorkers.size() > mSystemUpdateWorkersComplete);                
                mSystemUpdateWorkersComplete = 0;
            }
            else
            {
                system->VUpdate(dt, entityVec);
            }
        }
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

void World::OnWorkerCompleteCallback()
{
    mSystemUpdateWorkersComplete++;
}

///------------------------------------------------------------------------------------------------

World::World()
{
    mEntityComponentStore.reserve(ANTICIPATED_ENTITY_COUNT);
    
    auto threadWorkerCount = 1;
    for (auto i = 0U; i < threadWorkerCount; ++i)
    {
        mSystemUpdateWorkers.push_back(std::make_unique<SystemUpdateWorker>());
        mSystemUpdateWorkers.back()->StartWorker([&](){OnWorkerCompleteCallback();});
    }
}

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------