///------------------------------------------------------------------------------------------------
///  ConsoleCommandUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#include "ConsoleCommandUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

void RegisterConsoleCommand
(
    const StringId& consoleCommandName,
    const ConsoleCommand& consoleCommand
)
{
    auto& world = ecs::World::GetInstance();
    assert(world.HasSingletonComponent<ConsoleStateSingletonComponent>() && "Attempted to register console command but ConsoleStateSingletonComponent system was not found");    

    auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();
    consoleStateComponent.mRegisterdConsoleCommands[consoleCommandName] = consoleCommand;
}

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------