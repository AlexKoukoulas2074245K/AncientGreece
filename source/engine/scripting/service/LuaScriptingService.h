///------------------------------------------------------------------------------------------------
///  LuaScriptingService.h
///  Genesis
///
///  Created by Alex Koukoulas on 02/01/2020.
///------------------------------------------------------------------------------------------------

#ifndef LuaScriptingService_h
#define LuaScriptingService_h

///------------------------------------------------------------------------------------------------

#include "../../common/utils/StringUtils.h"
#include "../../engine/GenesisEngine.h"

#include <iostream>
#include <initializer_list>

///------------------------------------------------------------------------------------------------

struct lua_State;

///------------------------------------------------------------------------------------------------

typedef int(*lua_CFunction) (lua_State *L);

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace scripting
{

///------------------------------------------------------------------------------------------------
/// A class providing access to scripting opening/executing utilities.
class LuaScriptingService final
{        
    friend class genesis::GenesisEngine;

public:   
    /// The default method of getting a hold of this singleton.
    ///
    /// The single instance of this class will be lazily initialized
    /// the first time it is needed.
    /// @returns a reference to the single instance of this class.    
    static LuaScriptingService& GetInstance();

    ~LuaScriptingService();
    LuaScriptingService(const LuaScriptingService&) = delete;
    LuaScriptingService(LuaScriptingService&&) = delete;
    const LuaScriptingService& operator = (const LuaScriptingService&) = delete;
    LuaScriptingService& operator = (LuaScriptingService&&) = delete;    

    /// Runs a script with the given name.    
    /// @param[in] scriptName the name of the script to run (this is the filename of the music track without the extension).    
    void RunLuaScript(const std::string& scriptName) const;

    /// Reports a lua scripting error.
    /// @param[in] errorMessage the error message to display.
    void ReportLuaScriptError(const std::string& errorMessage) const;

    /// Binds a native function to the lua state.    
    /// @param[in] functionName the name that will be registered for the function on the lua state.   
    /// @param[in] functionDescription a description of the function's signature and return type, e.g. "f(a,b,c) -> int". Used 
    /// in the lua script generation to provide a list of bound native functions as comments at the top of the file.
    /// @param[in] function the function to be registered on the lua state.   
    void BindNativeFunctionToLua(const std::string& functionName, const std::string& functionDescription, lua_CFunction function) const;

    /// Returns the index of the top element in the stack. 
    ///
    /// Because indices start at 1, this result is equal to the number of elements in 
    /// the stack (and so 0 means an empty stack).
    int LuaGetIndexOfTopElement() const;

    /// Converts the Lua value at the given stack index to a long long int.
    /// @param[in] stackIndex the stack index to get the value from.
    /// @returns the long long value at the given stack index.
    long long LuaToIntegral(const int stackIndex) const;

    /// Converts the Lua value at the given stack index to a double.
    /// @param[in] stackIndex the stack index to get the value from.
    /// @returns the double value at the given stack index.
    double LuaToDouble(const int stackIndex) const;

    /// Converts the Lua value at the given stack index to a string.
    /// @param[in] stackIndex the stack index to get the value from.
    /// @returns an immutable string representation of the value at the given stack index.
    const char* LuaToString(const int stackIndex) const;

    /// Pushes an integer value to the Lua stack.    
    /// @param[in] value value to be pushed to the Lua stack.
    void LuaPushIntegral(const long long value) const;

    /// Pushes a double value to the Lua stack.    
    /// @param[in] value value to be pushed to the Lua stack.
    void LuaPushDouble(const double value) const;

    /// Pushes a boolean value to the Lua stack.    
    /// @param[in] value value to be pushed to the Lua stack.
    void LuaPushBoolean(const bool value) const;

    /// Pushes a string value to the Lua stack.    
    /// @param[in] value value to be pushed to the Lua stack.
    void LuaPushString(const std::string& value) const;

    /// Calls a Lua global function that takes no argument by name.    
    /// @param[in] globalFunctionName Lua function name to call.
    void LuaCallGlobalFunction(const std::string& globalFunctionName) const;

    /// Calls a Lua global function with a variable amount of arguments.    
    /// @tparam Arg type of the first function argument.
    /// @tparam Args rest of the types of the function arguments.
    /// @param[in] globalFunctionName the respective Lua function to call.
    /// @param[in] argc the number of arguments that the Lua function accepts.
    /// @param[in] firstArg the first argument the function accepts.
    /// @param[in] args the rest of the function arguments.
    template<class Arg, class... Args>
    void LuaCallGlobalFunction(const std::string& globalFunctionName, const int argc, const Arg& firstArg, Args... args) const
    {
        LuaGetGlobal(globalFunctionName);
        LuaPushArgument(firstArg, args...);
        LuaProcedureCall(argc);
    }

private:    
    LuaScriptingService() = default;
    
    void Initialize();

    // Internal lua get global wrapper.
    void LuaGetGlobal(const std::string& globalName) const;

    // Internal lua proc call wrapper.
    void LuaProcedureCall(const int argc) const;

    // Helper Push Argument specializations.
    template<class T, class... Args>
    void LuaPushArgument(const T& t, Args... args) const
    {
        LuaPushArgument(t);
        LuaPushArgument(args...);
    }

    template<class T>
    typename std::enable_if<std::is_same<T, std::string>::value>::type LuaPushArgument(const T& t) const
    {
        LuaPushString(t);
    }

    template<std::size_t N>
    void LuaPushArgument(const char(&value)[N]) const
    {
        LuaPushString(value);
    }

    template<class T>
    typename std::enable_if<std::is_integral<T>::value>::type LuaPushArgument(const T& t) const
    {
        LuaPushIntegral(t);
    }

    template<class T>
    typename std::enable_if<std::is_floating_point<T>::value>::type LuaPushArgument(const T& t) const
    {
        LuaPushDouble(t);
    }    

private:
    lua_State* mLuaState;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* LuaScriptingService_h */
