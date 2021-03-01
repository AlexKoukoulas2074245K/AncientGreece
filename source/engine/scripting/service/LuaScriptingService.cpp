///------------------------------------------------------------------------------------------------
///  LuaScriptingService.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 02/01/2020.
///------------------------------------------------------------------------------------------------

#include "LuaScriptingService.h"
#include "../../common/utils/OSMessageBox.h"
#include "../../resources/DataFileResource.h"
#include "../../resources/ResourceLoadingService.h"

extern "C" 
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------ 

namespace scripting
{

///------------------------------------------------------------------------------------------------ 

namespace
{
    static const std::string SCRIPT_FILE_EXTENSION = ".lua";
}

///------------------------------------------------------------------------------------------------ 

LuaScriptingService& LuaScriptingService::GetInstance()
{
    static LuaScriptingService instance;
    return instance;
}

///------------------------------------------------------------------------------------------------

LuaScriptingService::~LuaScriptingService()
{
    lua_close(mLuaState);
    mLuaState = nullptr;
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::RunLuaScript(const std::string& scriptName) const
{
    // Get actual script path
    const auto& scriptPath = resources::ResourceLoadingService::RES_SCRIPTS_ROOT + scriptName + SCRIPT_FILE_EXTENSION;

    // Run script
    lua_settop(mLuaState, 0); //empty the lua stack
    if (luaL_dofile(mLuaState, scriptPath.c_str())) 
    {       
        const auto& errorMessage = std::string(lua_tostring(mLuaState, -1));
        ReportLuaScriptError("Failed to load script:\n" + errorMessage);                
    }    
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::ReportLuaScriptError(const std::string& errorMessage) const
{
    ShowMessageBox(MessageBoxType::ERROR, "Script error", errorMessage);
    lua_pop(mLuaState, 1);
    assert(false);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::BindNativeFunctionToLua(const std::string& functionName, const std::string&, lua_CFunction function) const
{       
    lua_pushcfunction(mLuaState, function);
    lua_setglobal(mLuaState, functionName.c_str());
}

///------------------------------------------------------------------------------------------------

int LuaScriptingService::LuaGetIndexOfTopElement() const
{
    return lua_gettop(mLuaState);
}

///------------------------------------------------------------------------------------------------

long long LuaScriptingService::LuaToIntegral(const int stackIndex) const
{
    return lua_tointeger(mLuaState, stackIndex);
}

///------------------------------------------------------------------------------------------------

double LuaScriptingService::LuaToDouble(const int stackIndex) const
{
    return lua_tonumber(mLuaState, stackIndex);
}

///------------------------------------------------------------------------------------------------

const char* LuaScriptingService::LuaToString(const int stackIndex) const
{
    return lua_tostring(mLuaState, stackIndex);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaPushIntegral(const long long value) const
{
    lua_pushinteger(mLuaState, value);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaPushDouble(const double value) const
{
    lua_pushnumber(mLuaState, value);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaPushBoolean(const bool value) const
{
    lua_pushboolean(mLuaState, value);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaPushString(const std::string& value) const
{
    lua_pushstring(mLuaState, value.c_str());
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaCallGlobalFunction(const std::string& functionName) const
{
    LuaGetGlobal(functionName);
    LuaProcedureCall(0);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::Initialize()
{
    mLuaState = luaL_newstate();
    luaL_openlibs(mLuaState);
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaGetGlobal(const std::string& globalName) const
{
    lua_getglobal(mLuaState, globalName.c_str());
}

///------------------------------------------------------------------------------------------------

void LuaScriptingService::LuaProcedureCall(const int argc) const
{    
    if (lua_pcall(mLuaState, argc, 0, 0))
    {
        const auto& errorMessage = std::string(lua_tostring(mLuaState, -1));
        ReportLuaScriptError("Error running function: " + errorMessage);
    }
}

///------------------------------------------------------------------------------------------------

}

}