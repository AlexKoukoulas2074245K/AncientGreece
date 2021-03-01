///------------------------------------------------------------------------------------------------
///  ShaderStoreSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef ShaderStoreSingletonComponent_h
#define ShaderStoreSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../resources/ShaderResource.h"

#include <memory>
#include <unordered_map>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class ShaderStoreSingletonComponent final: public ecs::IComponent
{
public:
    tsl::robin_map<StringId, resources::ShaderResource, StringIdHasher> mShaders;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ShaderStoreSingletonComponent_h */
