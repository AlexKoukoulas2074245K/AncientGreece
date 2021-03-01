///------------------------------------------------------------------------------------------------
///  ConsoleManagementSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 06/01/2020.
///-----------------------------------------------------------------------------------------------

#include "ConsoleManagementSystem.h"
#include "../components/ConsoleStateSingletonComponent.h"
#include "../../input/utils/InputUtils.h"
#include "../../rendering/components/RenderableComponent.h"
#include "../../rendering/components/TextStringComponent.h"
#include "../../rendering/utils/Colors.h"
#include "../../rendering/utils/FontUtils.h"
#include "../../rendering/utils/MeshUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const glm::vec4 CONSOLE_TEXT_COLOR                    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    static const glm::vec3 CONSOLE_CURRENT_COMMAND_TEXT_POSITION = glm::vec3(-0.95f, -0.65f, -0.1f);

    static const StringId CONSOLE_TEXT_FONT_NAME          = StringId("console_font");
    static const StringId CONSOLE_BACKGROUND_ENTITY_NAME  = StringId("console_background");
    static const StringId CONSOLE_OPAQUENESS_UNIFORM_NAME = StringId("opaqueness");

    static const float CONSOLE_TEXT_SIZE       = 0.1f;
    static const float CONSOLE_DARKENING_SPEED = 0.1f;
    static const float MAX_OPAQUENESS          = 0.8f;
    
    static const int CONSOLE_MAX_LINES_VISIBLE = 15;

    static const char CONSOLE_PROMPT_CHARACTER = '>';
}

///-----------------------------------------------------------------------------------------------

ConsoleManagementSystem::ConsoleManagementSystem()
    : BaseSystem()
{       
    ecs::World::GetInstance().SetSingletonComponent<ConsoleStateSingletonComponent>(std::make_unique<ConsoleStateSingletonComponent>());    
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::VUpdate(const float, const std::vector<ecs::EntityId>&) const
{
    CreateConsoleBackgroundEntityIfNotAlive();
    HandleConsoleSpecialInput();
    HandleConsoleBackgroundAnimation();
    HandleConsoleTextRendering();
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::CreateConsoleBackgroundEntityIfNotAlive() const
{
    if (ecs::World::GetInstance().FindEntityWithName(CONSOLE_BACKGROUND_ENTITY_NAME) == ecs::NULL_ENTITY_ID)
    {
        rendering::LoadAndCreateGuiSprite("gui_base", "debug_square", StringId("console"), glm::vec3(0.0f, 0.0f, 0.0f), CONSOLE_BACKGROUND_ENTITY_NAME);
    }
}

void ConsoleManagementSystem::HandleConsoleSpecialInput() const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<debug::ConsoleStateSingletonComponent>();

    // Handling console open/close
    if (input::IsActionTypeKeyTapped(input::InputActionType::CONSOLE_TOGGLE))
    {
        consoleStateComponent.mEnabled = !consoleStateComponent.mEnabled;

        if (consoleStateComponent.mEnabled)
        {   
            consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER;
            SDL_StartTextInput();
        }
        else
        {
            SDL_StopTextInput();
            consoleStateComponent.mCurrentCommandTextBuffer.clear();

            for (const auto& pastConsoleTextStringEntityIds : consoleStateComponent.mPastConsoleTextStringEntityIds)
            {
                rendering::DestroyRenderedText(pastConsoleTextStringEntityIds);
            }

            consoleStateComponent.mPastConsoleTextStringEntityIds.clear();
        }
    }
    // Handle console backspace
    else if (input::IsActionTypeKeyTapped(input::InputActionType::BACKSPACE_KEY))
    {
        if (consoleStateComponent.mEnabled && consoleStateComponent.mCurrentCommandTextBuffer.size() > 1)
        {
            auto& consoleCurrentCommandText = consoleStateComponent.mCurrentCommandTextBuffer;
            consoleCurrentCommandText       = consoleCurrentCommandText.substr(0, consoleCurrentCommandText.size() - 1);            
        }
    }
    // Handle previous command cycling
    else if (input::IsActionTypeKeyTapped(input::InputActionType::UP_ARROW_KEY))
    {
        if (consoleStateComponent.mEnabled && consoleStateComponent.mCommandHistory.size() > 0)
        {
            consoleStateComponent.mCommandHistoryIndex = math::Min(static_cast<int>(consoleStateComponent.mCommandHistory.size()) - 1, consoleStateComponent.mCommandHistoryIndex + 1);            
            const auto& targetCommandText = consoleStateComponent.mCommandHistory.at(consoleStateComponent.mCommandHistoryIndex);
            consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER + targetCommandText;
        }
    }
    // Handle next command cycling
    else if (input::IsActionTypeKeyTapped(input::InputActionType::DOWN_ARROW_KEY))
    {
        if (consoleStateComponent.mEnabled && consoleStateComponent.mCommandHistory.size() > 0)
        {
            consoleStateComponent.mCommandHistoryIndex = math::Max(0, consoleStateComponent.mCommandHistoryIndex - 1);
            const auto& targetCommandText = consoleStateComponent.mCommandHistory.at(consoleStateComponent.mCommandHistoryIndex);
            consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER + targetCommandText;
        }
    }
    // Handle command execution
    else if (input::IsActionTypeKeyTapped(input::InputActionType::ENTER_KEY))
    {
        if (consoleStateComponent.mEnabled)
        {
            ExecuteCommand();
        }
    }
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::HandleConsoleBackgroundAnimation() const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<ConsoleStateSingletonComponent>();
    if (consoleStateComponent.mEnabled)
    {
        // dt is not used in the opaqueness calculation since frozen when console opens
        consoleStateComponent.mBackgroundOpaqueness += CONSOLE_DARKENING_SPEED;
        if (consoleStateComponent.mBackgroundOpaqueness > MAX_OPAQUENESS)
        {
            consoleStateComponent.mBackgroundOpaqueness = MAX_OPAQUENESS;
        }
    }
    else
    {
        // dt is not used in the opaqueness calculation since frozen when console opens
        consoleStateComponent.mBackgroundOpaqueness -= CONSOLE_DARKENING_SPEED;
        if (consoleStateComponent.mBackgroundOpaqueness < 0.0f)
        {
            consoleStateComponent.mBackgroundOpaqueness = 0.0f;
        }
    }

    
    const auto consoleBackgroundEntity = ecs::World::GetInstance().FindEntityWithName(CONSOLE_BACKGROUND_ENTITY_NAME);
    if (consoleBackgroundEntity != ecs::NULL_ENTITY_ID)
    {
        auto& consoleBackgroundRenderableComponent = ecs::World::GetInstance().GetComponent<rendering::RenderableComponent>(consoleBackgroundEntity);
        consoleBackgroundRenderableComponent.mShaderUniforms.mShaderFloatUniforms[CONSOLE_OPAQUENESS_UNIFORM_NAME] = consoleStateComponent.mBackgroundOpaqueness;
    }
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::HandleConsoleTextRendering() const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<ConsoleStateSingletonComponent>();

    if (consoleStateComponent.mCurrentCommandRenderedTextEntityId == ecs::NULL_ENTITY_ID || IsCurrentCommandRenderedTextOutOfDate())
    {
        if (consoleStateComponent.mCurrentCommandRenderedTextEntityId != ecs::NULL_ENTITY_ID)
        {
            rendering::DestroyRenderedText(consoleStateComponent.mCurrentCommandRenderedTextEntityId);            
        }

        consoleStateComponent.mCurrentCommandRenderedTextEntityId = rendering::RenderText
        (
            consoleStateComponent.mCurrentCommandTextBuffer,
            CONSOLE_TEXT_FONT_NAME,
            CONSOLE_TEXT_SIZE,
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION,
            CONSOLE_TEXT_COLOR
        );        
    }    
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::ExecuteCommand() const
{
    const auto& world = ecs::World::GetInstance();
    auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();
        
    // Add command-to-be-executed to past console text
    AddTextStringToConsolePastText(consoleStateComponent.mCurrentCommandRenderedTextEntityId);          
    
    // Add to command history and execute command if nonempty text 
    if (consoleStateComponent.mCurrentCommandTextBuffer.size() != 1)
    {        
        AddCommandTextToCommandHistory(consoleStateComponent.mCurrentCommandTextBuffer.substr(1));

        const auto& commandSplitBySpace = StringSplit(consoleStateComponent.mCurrentCommandTextBuffer.substr(1), ' ');

        // If command not found let user know
        if (consoleStateComponent.mRegisterdConsoleCommands.count(commandSplitBySpace[0]) == 0)
        {
            // Render command not found text
            AddTextStringToConsolePastText(rendering::RenderText
            (
                "Command not found.",
                CONSOLE_TEXT_FONT_NAME,
                CONSOLE_TEXT_SIZE, 
                CONSOLE_CURRENT_COMMAND_TEXT_POSITION, 
                rendering::colors::RED
            ));
            AddTextStringToConsolePastText(rendering::RenderText
            (
                "Type \"commands\" to see all available ones.",
                CONSOLE_TEXT_FONT_NAME,
                CONSOLE_TEXT_SIZE,
                CONSOLE_CURRENT_COMMAND_TEXT_POSITION,
                rendering::colors::RED
            ));
        }
        // Otherwise execute to command logic and print result if any
        else
        {
            // Execute command
            const auto& commandResult = consoleStateComponent.mRegisterdConsoleCommands.at(StringId(commandSplitBySpace[0]))(commandSplitBySpace);
            if (!commandResult.mResponseText.empty())
            {
                // Print result text line by line
                const auto& commandResultSplitByNewline = StringSplit(commandResult.mResponseText, '\n');
                for (const auto& commandResultTextLine: commandResultSplitByNewline)
                {
                    AddTextStringToConsolePastText(rendering::RenderText
                    (
                        commandResultTextLine,
                        CONSOLE_TEXT_FONT_NAME,
                        CONSOLE_TEXT_SIZE,
                        CONSOLE_CURRENT_COMMAND_TEXT_POSITION,
                        commandResult.mSuccess ? rendering::colors::GREEN : rendering::colors::RED                    
                    ));
                }
            }
        }
    }    
    
    // Move past console text up
    RepositionPastConsoleTextStrings();

    // Reset current command text
    consoleStateComponent.mCurrentCommandRenderedTextEntityId = ecs::NULL_ENTITY_ID;
    consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER;
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::AddTextStringToConsolePastText(const ecs::EntityId commandStringTextEntityId) const
{
    const auto& world = ecs::World::GetInstance();
    auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();

    // Add text string to past console text
    consoleStateComponent.mPastConsoleTextStringEntityIds.push_back(commandStringTextEntityId);

    // Remove overflowing past text strings (circular buffer style)
    if (consoleStateComponent.mPastConsoleTextStringEntityIds.size() > CONSOLE_MAX_LINES_VISIBLE)
    {
        rendering::DestroyRenderedText(consoleStateComponent.mPastConsoleTextStringEntityIds.front());
        consoleStateComponent.mPastConsoleTextStringEntityIds.erase(consoleStateComponent.mPastConsoleTextStringEntityIds.begin());
    }
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::AddCommandTextToCommandHistory(const std::string& commandText) const
{
    const auto& world = ecs::World::GetInstance();
    auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();

    if (commandText.size() != 1)
    {
        consoleStateComponent.mCommandHistory.insert(consoleStateComponent.mCommandHistory.begin(), commandText);
        consoleStateComponent.mCommandHistoryIndex = -1;
    }
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::RepositionPastConsoleTextStrings() const
{
    const auto& world = ecs::World::GetInstance();
    auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();

    for (auto i = static_cast<int>(consoleStateComponent.mPastConsoleTextStringEntityIds.size()) - 1; i >= 0; --i)
    {
        const auto positionDisplacementIndex = consoleStateComponent.mPastConsoleTextStringEntityIds.size() - i;
        const auto pastConsoleTextStringEntityId = consoleStateComponent.mPastConsoleTextStringEntityIds.at(static_cast<size_t>(i));
        const auto targetPosition = glm::vec3
        (
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION.x,
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION.y + positionDisplacementIndex * CONSOLE_TEXT_SIZE,
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION.z
        );

        rendering::SetTextPosition(pastConsoleTextStringEntityId, targetPosition);
    }
}

///-----------------------------------------------------------------------------------------------

bool ConsoleManagementSystem::IsCurrentCommandRenderedTextOutOfDate() const
{
    const auto& world = ecs::World::GetInstance();
    const auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();
    const auto& renderedTextStringComponent = world.GetComponent<rendering::TextStringComponent>(consoleStateComponent.mCurrentCommandRenderedTextEntityId);

    std::string renderedCommandText = "";
    for (const auto& characterEntry : renderedTextStringComponent.mTextCharacterEntities)
    {
        renderedCommandText += characterEntry.mCharacter;
    }

    return renderedCommandText != consoleStateComponent.mCurrentCommandTextBuffer;
}

///-----------------------------------------------------------------------------------------------

}

}
