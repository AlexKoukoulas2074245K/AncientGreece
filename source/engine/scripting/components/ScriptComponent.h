///------------------------------------------------------------------------------------------------
///  ScriptComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef ScriptComponent_h
#define ScriptComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/StringUtils.h"

#include <memory>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace scripting
{

///-----------------------------------------------------------------------------------------------

/// Denotes the script type, i.e. a continuously updated script, or a one-off.
///
/// single execution scripts are expected to have an "Execute" function, where
/// as the continuously updated scripts are expected to have an "Update" function
/// (The genLuaScript script takes care of all this).
enum class ScriptType
{
    SINGLE_EXECUTION, CONTINUOUS_EXECUTION
};

///-----------------------------------------------------------------------------------------------

class ScriptComponent final : public ecs::IComponent
{
public:
    StringId mScriptName   = StringId();
    ScriptType mScriptType = ScriptType::SINGLE_EXECUTION;    
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ScriptComponent_h */