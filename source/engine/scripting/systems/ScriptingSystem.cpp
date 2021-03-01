///------------------------------------------------------------------------------------------------
///  ScriptingSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 08/01/2020.
///-----------------------------------------------------------------------------------------------

#include "ScriptingSystem.h"
#include "../components/ScriptComponent.h"
#include "../service/LuaScriptingService.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace scripting
{

///-----------------------------------------------------------------------------------------------
  
static tsl::robin_map<ScriptType, std::string> sScriptTypeToLuaFuncName =
{
    { ScriptType::CONTINUOUS_EXECUTION, "Update" },
    { ScriptType::SINGLE_EXECUTION, "Execute" }
};

///-----------------------------------------------------------------------------------------------

ScriptingSystem::ScriptingSystem()
{
}

///-----------------------------------------------------------------------------------------------

void ScriptingSystem::VUpdate(const float dt, const std::vector<ecs::EntityId>& entitiesToProcess) const
{
    static float dtAccum = 0.0f;
    dtAccum += dt;

    for (const auto& entityId : entitiesToProcess)
    {
        const auto& scriptComponent = ecs::World::GetInstance().GetComponent<ScriptComponent>(entityId);
        LuaScriptingService::GetInstance().RunLuaScript(scriptComponent.mScriptName.GetString());
        LuaScriptingService::GetInstance().LuaCallGlobalFunction(sScriptTypeToLuaFuncName.at(scriptComponent.mScriptType), 3, entityId, dt, dtAccum);
        
    }
}

///-----------------------------------------------------------------------------------------------

}

}
