///------------------------------------------------------------------------------------------------
///  ConsoleManagementSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef ConsoleManagementSystem_h
#define ConsoleManagementSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------
class ConsoleManagementSystem final : public ecs::BaseSystem<ecs::NullComponent>
{
public:
    ConsoleManagementSystem();

    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;
    void CreateConsoleBackgroundEntityIfNotAlive() const;
    void HandleConsoleSpecialInput() const;
    void HandleConsoleBackgroundAnimation() const;
    void HandleConsoleTextRendering() const;
    void ExecuteCommand() const;
    
    void AddTextStringToConsolePastText(const ecs::EntityId commandStringTextEntityId) const;
    void AddCommandTextToCommandHistory(const std::string& commandText) const;
    void RepositionPastConsoleTextStrings() const;
    bool IsCurrentCommandRenderedTextOutOfDate() const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ConsoleManagementSystem_h */
