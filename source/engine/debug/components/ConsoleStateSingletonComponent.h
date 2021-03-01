///------------------------------------------------------------------------------------------------
///  ConsoleStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 06/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef ConsoleStateSingletonComponent_h
#define ConsoleStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

#include <functional>
#include <string>
#include <tsl/robin_map.h>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

struct ConsoleCommandResult
{
    ConsoleCommandResult(const bool success, const std::string responseText = "")
        : mSuccess(success)
        , mResponseText(responseText)        
    {
    }

    const bool mSuccess;
    const std::string mResponseText;
};

///-----------------------------------------------------------------------------------------------

using ConsoleCommand = std::function<ConsoleCommandResult(const std::vector<std::string>&)>;

///-----------------------------------------------------------------------------------------------

class ConsoleStateSingletonComponent final: public ecs::IComponent
{
public:
    tsl::robin_map<StringId, ConsoleCommand, StringIdHasher> mRegisterdConsoleCommands;
    std::vector<std::string> mCommandHistory;
    std::vector<ecs::EntityId> mPastConsoleTextStringEntityIds;
    ecs::EntityId mCurrentCommandRenderedTextEntityId = ecs::NULL_ENTITY_ID;    
    std::string mCurrentCommandTextBuffer             = "";
    float mBackgroundOpaqueness                       = 0.0f;
    int mCommandHistoryIndex                          = -1;
    bool mEnabled                                     = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ConsoleStateSingletonComponent_h */
