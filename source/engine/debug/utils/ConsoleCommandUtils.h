///------------------------------------------------------------------------------------------------
///  ConsoleCommandUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef ConsoleCommandUtils_h
#define ConsoleCommandUtils_h

///-----------------------------------------------------------------------------------------------

#include "../components/ConsoleStateSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------
/// Registers and binds a console command function to a given name.
/// @param[in] consoleCommandName the name to bind the console command function to.
/// @param[in] consoleCommand the console command function
void RegisterConsoleCommand
(
    const StringId& consoleCommandName,
    const ConsoleCommand& consoleCommand
);

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ConsoleCommandUtils_h */

