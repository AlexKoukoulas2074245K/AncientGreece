///------------------------------------------------------------------------------------------------
///  DefaultEngineExportableFunctions.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 03/10/2019.
///------------------------------------------------------------------------------------------------

#include "DefaultEngineExportableFunctions.h"
#include "service/LuaScriptingService.h"
#include "systems/ScriptingSystem.h"
#include "../common/components/TransformComponent.h"

#include <unordered_set>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace scripting
{

///------------------------------------------------------------------------------------------------

void BindDefaultEngineFunctionsToLua()
{
    using scripting::LuaScriptingService;

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("CreateEntity", "CreateEntity([optional]entityName) -> entityId", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 0)
        {
            luaScriptingService.LuaPushIntegral(ecs::World::GetInstance().CreateEntity());
        }
        else if (stackSize == 1)
        {
            const auto entityName = StringId(luaScriptingService.LuaToString(1));
            luaScriptingService.LuaPushIntegral(ecs::World::GetInstance().CreateEntity(entityName));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 0 or 1) when calling CreateEntity");
        }

        return 1;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("FindEntityWithName", "FindEntityWithName(entityName) -> entityId", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityName = StringId(luaScriptingService.LuaToString(1));
            luaScriptingService.LuaPushIntegral(ecs::World::GetInstance().FindEntityWithName(entityName));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling FindEntityWithName");
        }

        return 1;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("DestroyEntity", "DestroyEntity(entityId) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            ecs::World::GetInstance().DestroyEntity(entityId);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling DestroyEntity");
        }

        return 0;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("GetEntityPosition", "GetEntityPosition(entityId) -> x,y,z", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto& transformComponent = ecs::World::GetInstance().GetComponent<TransformComponent>(entityId);
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.x));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.y));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.z));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling GetEntityPosition");
        }

        return 3;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("SetEntityPosition", "SetEntityPosition(entityId, x, y, z) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 4)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto positionX = luaScriptingService.LuaToDouble(2);
            const auto positionY = luaScriptingService.LuaToDouble(3);
            const auto positionZ = luaScriptingService.LuaToDouble(4);

            auto& transformComponent = ecs::World::GetInstance().GetComponent<TransformComponent>(entityId);
            transformComponent.mPosition = glm::vec3(positionX, positionY, positionZ);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 4) when calling SetEntityPosition");
        }

        return 0;
    });
    
    LuaScriptingService::GetInstance().BindNativeFunctionToLua("GetEntityRotation", "GetEntityRotation(entityId) -> rx,ry,rz", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto& transformComponent = ecs::World::GetInstance().GetComponent<TransformComponent>(entityId);
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mRotation.x));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mRotation.y));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mRotation.z));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling GetEntityPosition");
        }

        return 3;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("SetEntityRotation", "SetEntityRotation(entityId, rx, ry, rz) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 4)
        {
            const auto entityId  = luaScriptingService.LuaToIntegral(1);
            const auto rotationX = luaScriptingService.LuaToDouble(2);
            const auto rotationY = luaScriptingService.LuaToDouble(3);
            const auto rotationZ = luaScriptingService.LuaToDouble(4);

            auto& transformComponent = ecs::World::GetInstance().GetComponent<TransformComponent>(entityId);
            transformComponent.mRotation = glm::vec3(rotationX, rotationY, rotationZ);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 4) when calling SetEntityPosition");
        }

        return 0;
    });
}

///------------------------------------------------------------------------------------------------

}
}

///------------------------------------------------------------------------------------------------
