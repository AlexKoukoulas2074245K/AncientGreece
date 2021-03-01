///------------------------------------------------------------------------------------------------
///  ECS.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef ECS_h
#define ECS_h

///------------------------------------------------------------------------------------------------

//#define CONSOLE_ENABLED_ON_RELEASE

///------------------------------------------------------------------------------------------------

#include "common/utils/StringUtils.h"
#include "common/utils/TypeTraits.h"

#include <algorithm>
#include <array>
#include <bitset>        
#include <cassert>
#include <map>
#include <memory>
#include <tsl/robin_map.h>
#include <vector>        

///------------------------------------------------------------------------------------------------

namespace genesis
{ 

namespace ecs
{

/// Max component type count allowed
static constexpr int MAX_COMPONENTS = 32;

/// Initial guess for the average entities handled
/// so that multiple resizes won't be needed
static constexpr int ANTICIPATED_ENTITY_COUNT = 1000;

/// Null entity ID
static constexpr long long NULL_ENTITY_ID = 0LL;

///------------------------------------------------------------------------------------------------

class World;
class ISystem;
class IComponent;

using ComponentMask   = std::bitset<MAX_COMPONENTS>;
using ComponentTypeId = int;
using SystemTypeId    = int;
using EntityId        = long long;

///------------------------------------------------------------------------------------------------

struct ComponentTypeIdHasher
{
    std::size_t operator()(const ComponentTypeId& key) const
    {
        return static_cast<std::size_t>(key);
    }
};

struct EntityIdHasher
{
    std::size_t operator()(const EntityId& key) const
    {
        return static_cast<std::size_t>(key);
    }
};

///------------------------------------------------------------------------------------------------
/// Base class of all components in the engine. All custom components needs to inherit from
/// this class.
class IComponent
{     
public:
    IComponent() = default;
    virtual ~IComponent() = default;
    
    IComponent(const IComponent&) = delete;
    IComponent& operator=(const IComponent&) = delete;
    
};

///------------------------------------------------------------------------------------------------
/// To be used when a system doesn't care about any component types in its update
class NullComponent final: public IComponent
{
};

///------------------------------------------------------------------------------------------------
/// The kernel of the ECS engine. Manages all registered systems and entities.
class World final
{
public:     
    /// The default method of getting a hold of this singleton.
    ///
    /// The single instance of this class will be lazily initialized
    /// the first time it is needed.
    /// @returns a reference to the single instance of this class.    
    static World& GetInstance();

    /// @returns a map containing the system name strings mapped to their current update times in miliseconds.
    const tsl::robin_map<StringId, long long, StringIdHasher>& GetSystemUpdateTimes() const;

    /// Performs a single update of the world simulation.
    /// @param[in] dt the inter-frame delta time in seconds.
    void Update(const float dt);

    /// Adds a system to the world and takes ownership of it
    /// @param[in] system the system instance to add to the world and take ownership over.
    void AddSystem(std::unique_ptr<ISystem> system);

    /// Creates an entity and returns its corresponding entity id.     
    /// @returns the entity id of the newly constructed entity.
    EntityId CreateEntity();
    
    /// Creates a named entity and returns its corresponding entity id.
    /// @param[in] name the name to associate the entity with.
    /// @returns the entity id of the newly constructed entity.
    EntityId CreateEntity(const StringId& name);
    
    /// Checks whether the entity with the respective entity id exists in the world.
    ///
    /// Should be used before accessing components of entities indirectly; e.g.
    /// in a system that uses components that store entity id references
    /// @param[in] entityId the entity id to check for.
    bool HasEntity(const EntityId entityId) const;
    
    /// Removes the given entity from the world.
    ///
    /// Internally simply clears the component store for the given entity since all entities
    /// with no attached components will be cleaned up at the beginning of the next frame.
    /// @param[in] entityId the entity with this id will be destroyed.
    void DestroyEntity(const EntityId entityId);
    
    /// Removes all entities with the given ids from the world.
    /// @param[in] entityIds the ids of all entities to be destroyed.
    void DestroyEntities(const std::vector<EntityId>& entityIds);
    
    /// Finds and returns the first entity found with the name provided.
    /// @param[in] name the name to search for the entities with.
    /// @returns the entity id of the entity found, or NULL_ENTITY_ID otherwise
    EntityId FindEntityWithName(const StringId& entityName) const;
    
    /// Finds and returns all entities found with the name provided.
    /// @param[in] name the name to search for the entity with.
    /// @returns a vector of all entity ids found
    std::vector<EntityId> FindAllEntitiesWithName(const StringId& entityName) const;
    
    /// Returns the current entity count.
    /// @returns the current entity count in this world
    std::size_t GetEntityCount() const;
    
    /// Gets the respective component from the entity with the given entity id.
    ///
    /// The accessor will fail silently if the entity does not have a component of the respective component class.
    /// Always test for component ownership, unless certain that the entity has one. @see HasComponent() before using this method
    /// @tparam ComponentType the derived component type class to poll the entity for.
    /// @param[in] entityId the entity with the respective id to get the component from.
    /// @returns the component with the respective component type belonging to this entity.
    template<class ComponentType>
    [[nodiscard]] inline ComponentType& GetComponent(const EntityId entityId) const
    {        
        assert(entityId != NULL_ENTITY_ID &&
            "Requested a component from NULL_ENTITY_ID");

        assert(mEntityComponentStore.count(entityId) != 0 &&
            "Entity does not exist in the world");

        const auto componentTypeId = GetTypeHash<ComponentType>();
        const auto& entityEntry = mEntityComponentStore.at(entityId);

        assert(entityEntry.mComponents[componentTypeId] &&
            "Component is not present in this entity's component store");
        
        return static_cast<ComponentType&>(*entityEntry.mComponents[componentTypeId]);
    }

    /// Checks whether the given entity has a component of the given component class type.
    /// @tparam ComponentType the derived component type class to poll the entity for.
    /// @param[in] entityId the entity with the respective id to check component ownership from.
    /// returns whether the entity has a component of the respective type.
    template<class ComponentType>
    inline bool HasComponent(const EntityId entityId) const
    {
        assert(entityId != NULL_ENTITY_ID &&
            "Component check from NULL_ENTITY_ID");

        assert(mEntityComponentStore.count(entityId) != 0 &&
            "Entity does not exist in the world");

        const auto componentTypeId = GetTypeHash<ComponentType>();
        return mEntityComponentStore.at(entityId).mComponents[componentTypeId] != nullptr;
    }

    /// Adds and <b>takes ownership</b> of the given component and adds it to the entity with the given id.
    /// @tparam ComponentType the derived component type.
    /// @param[in] entityId the entity with the respective id to check component ownership from.
    /// @param[in] component the pointer to the component instance to be added to the entity.
    template<class ComponentType>
    inline void AddComponent(const EntityId entityId, std::unique_ptr<IComponent> component)
    {
        assert(entityId != NULL_ENTITY_ID &&
            "Component addition for NULL_ENTITY_ID");

        assert(mEntityComponentStore.count(entityId) != 0 &&
            "Entity does not exist in the world");

        const auto componentTypeId = GetTypeHash<ComponentType>();

        auto& entityEntry = mEntityComponentStore.at(entityId);
        assert(entityEntry.mComponents[componentTypeId] == nullptr &&
            "Component is already present in this entity's component store");
        
        entityEntry.mComponents[componentTypeId] = std::move(component);
        entityEntry.mMask |= 1 << componentTypeId;
        
        OnEntityChanged(entityId, entityEntry.mMask);
    }

    /// Removes the component with the given type from the entity with the given entity id.
    /// @tparam ComponentType the derived component type class to remove from the entity.
    /// @param[in] entityId the entity with the respective id to check component ownership from.    
    template<class ComponentType>
    inline void RemoveComponent(const EntityId entityId)
    {
        assert(entityId != NULL_ENTITY_ID &&
            "Component removal from NULL_ENTITY_ID");

        assert(mEntityComponentStore.count(entityId) != 0 &&
            "Entity does not exist in the world");

        const auto componentTypeId = GetTypeHash<ComponentType>();
        auto& entityEntry = mEntityComponentStore.at(entityId);

        assert(entityEntry.mComponents[componentTypeId] &&
            "Component is not present in this entity's component store");
        
        entityEntry.mComponents[componentTypeId] = nullptr;
        entityEntry.mMask ^= 1 << componentTypeId;
        
        OnEntityChanged(entityId, entityEntry.mMask);
    }
    
    /// Get the registered singleton component with the given type.
    /// @tparam ComponentType the singleton component type class to return.
    /// @returns the singleton component found
    template<class ComponentType>
    [[nodiscard]] inline ComponentType& GetSingletonComponent() const
    {
        static_assert(std::is_base_of<IComponent, ComponentType>::value,
            "ComponentType does not derive from IComponent");

        const auto componentTypeId = GetTypeHash<ComponentType>();
        return static_cast<ComponentType&>(*mSingletonComponents.at(componentTypeId));
    }

    /// Checks whether a singleton component of the given type exists.
    /// @tparam ComponentType the singleton component type class to check.
    /// @returns whether a singleton component of the given type exists.
    template<class ComponentType>
    inline bool HasSingletonComponent() const
    {
        static_assert(std::is_base_of<IComponent, ComponentType>::value,
            "ComponentType does not derive from IComponent");

        const auto componentTypeId = GetTypeHash<ComponentType>();
        return mSingletonComponents.count(componentTypeId) != 0;
    }    

    /// Takes ownership of the given singleton component and sets it in the world.
    ///
    /// It is intended that there should be only <b>1</b> singleton component of a given type in the world at all times.
    /// @tparam ComponentType the singleton component type class to take ownership of.
    /// @param[in] component the singleton component instance to take ownership of.
    template<class ComponentType>
    inline void SetSingletonComponent(std::unique_ptr<IComponent> component)
    {
        static_assert(std::is_base_of<IComponent, ComponentType>::value,
            "ComponentType does not derive from IComponent");

        const auto componentTypeId = GetTypeHash<ComponentType>();
        assert(mSingletonComponents.count(componentTypeId) == 0 &&
            "A Singleton component of the specified type already exists in the world");

        mSingletonComponents[componentTypeId] = std::move(component);
    }

    /// Removes the singleton component with the respective type from the world.    
    /// @tparam ComponentType the singleton component type class to remove. 
    template<class ComponentType>
    inline void RemoveSingletonComponent()
    {
        static_assert(std::is_base_of<IComponent, ComponentType>::value,
            "ComponentType does not derive from IComponent");
                
        const auto componentTypeId = GetTypeHash<ComponentType>();
        assert(mSingletonComponents.count(componentTypeId) != 0 &&
            "A Singleton component of the specified type does not exist");

        mSingletonComponents.erase(componentTypeId);
    }    
    
    /// Calculates the bit mask of the given template arguments.
    /// @tparam FirstUtilizedComponentType a component's type class
    template<class FirstUtilizedComponentType>
    [[nodiscard]] inline ComponentMask CalculateComponentUsageMask()
    {
        static_assert(std::is_base_of<IComponent, FirstUtilizedComponentType>::value,
            "Attempted to extract mask from class not derived from IComponent");
        
        return 1 << GetTypeHash<FirstUtilizedComponentType>();
    }

    /// Calculates the bit mask of the given template arguments.
    /// @tparam FirstUtilizedComponentType a component's type class
    /// @tparam SecondUtilizedComponentType a component's type class
    template<class FirstUtilizedComponentType, class SecondUtilizedComponentType, class ...RestUtilizedComponentTypes>
    [[nodiscard]] inline ComponentMask CalculateComponentUsageMask()
    {
        static_assert(std::is_base_of<IComponent, FirstUtilizedComponentType>::value,
            "Attempted to extract mask from class not derived from IComponent");

        const auto componentTypeId = GetTypeHash<FirstUtilizedComponentType>();

        return ComponentMask(1 << componentTypeId) |
            CalculateComponentUsageMask<SecondUtilizedComponentType, RestUtilizedComponentTypes...>();
    }
    
    /// Explicit specialization for NullComponent to return a full component mask.
    template<>
    [[nodiscard]] inline ComponentMask CalculateComponentUsageMask<NullComponent>()
    {
        return ~ComponentMask();
    }

private:        
    /// Reserves space for the anticipated entity count.
    World();

    /// Removes all entities with no components currently attached to them.
    void RemoveEntitiesWithoutAnyComponents();
    
    /// Adjusts the systems' entities to process accordingly
    /// @param[in] entityId the entity that has changed
    /// @param[in] newComponentMask the new component mask of the entity
    void OnEntityChanged(const EntityId entityId, const ComponentMask& newComponentMask);
    
private:
    struct EntityEntry
    {
        std::array<std::unique_ptr<IComponent>, MAX_COMPONENTS> mComponents;
        ComponentMask mMask;
    };

    using EntityComponentStoreMap = tsl::robin_map<EntityId, EntityEntry, EntityIdHasher>;
    using ComponentMap = tsl::robin_map<ComponentTypeId, std::unique_ptr<IComponent>, ComponentTypeIdHasher>;
    
    EntityComponentStoreMap mEntityComponentStore;
    ComponentMap            mSingletonComponents;
               
    tsl::robin_map<StringId, long long, StringIdHasher> mSystemUpdateToDuration;
    tsl::robin_map<std::type_index, std::vector<EntityId>> mEntitiesToUpdatePerSystem;
    
    std::vector<std::unique_ptr<ISystem>> mSystems;

    EntityId mEntityCounter = 1LL;
};

///------------------------------------------------------------------------------------------------

class ISystem
{
    friend class World;
public:
    ISystem() = default;
    virtual ~ISystem() = default;
    ISystem(const ISystem&) = delete;
    const ISystem& operator = (const ISystem&) = delete;
    
private:
    [[nodiscard]] virtual inline bool ShouldProcessComponentMask(const ComponentMask& componentMask) const = 0;
    
    /// Main system update method. To be implemented by all systems
    /// @param[in] dt the delta-time in seconds that has elapsed since the last frame
    /// @param[in] entitiesToProcess the entities that match this system's signature (mask) and that should be processed
    virtual void VUpdate(const float dt, const std::vector<EntityId>& entitiesToProcess) const = 0;
    
private:
    StringId mSystemName;
};

///------------------------------------------------------------------------------------------------
/// Base class of all systems in the ECS engine. All systems need to inherit from this class.
template<class... UtilizedComponentTypes>
class BaseSystem: public ISystem
{
public:
    BaseSystem()
        : mComponentUsageMask(World::GetInstance().CalculateComponentUsageMask<UtilizedComponentTypes...>())
    {
    }
        
    virtual ~BaseSystem() = default;
    BaseSystem(const BaseSystem&) = delete;
    const BaseSystem& operator = (const BaseSystem&) = delete;  
        
private:
    // Determines whether the given component mask should be processed by this system
    [[nodiscard]] inline bool ShouldProcessComponentMask(const ComponentMask& componentMask) const override
    {
        return (componentMask & mComponentUsageMask) == mComponentUsageMask;
    };

private:
    ComponentMask mComponentUsageMask;
    
};

}

}

///------------------------------------------------------------------------------------------------

#endif /* ECS_h */
