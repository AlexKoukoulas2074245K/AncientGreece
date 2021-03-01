///------------------------------------------------------------------------------------------------
///  DebugViewManagementSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#include "DebugViewManagementSystem.h"
#include "../components/DebugViewStateSingletonComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../rendering/components/LightStoreSingletonComponent.h"
#include "../../rendering/components/RenderableComponent.h"
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
    static const glm::vec3 FPS_TEXT_POSITION                    = glm::vec3(0.55f, 0.8f, 0.0f);
    static const glm::vec3 FPS_NUMBER_POSITION                  = glm::vec3(0.75f, 0.8f, 0.0f);
    static const glm::vec3 ENTITY_COUNT_TEXT_POSITION           = glm::vec3(0.38f, 0.7f, 0.0f);
    static const glm::vec3 ENTITY_COUNT_NUMBER_POSITION         = glm::vec3(0.75f, 0.7f, 0.0f);
    static const glm::vec3 SYSTEM_NAMES_STARTING_POSITION       = glm::vec3(-0.3f, 0.6f, 0.0f);
    static const glm::vec3 SYSTEM_UPDATE_TIME_STARTING_POSITION = glm::vec3(0.6f, 0.6f, 0.0f);
    static const glm::vec3 DEBUG_LIGHT_SCALE                    = glm::vec3(0.1f, 0.1f, 0.1f);

    static const StringId TEXT_FONT_NAME         = StringId("console_font");
    static const StringId DEBUG_LIGHT_ASSET_NAME = StringId("debug_light");

    static const float TEXT_SIZE         = 0.06f;
}

///-----------------------------------------------------------------------------------------------

DebugViewManagementSystem::DebugViewManagementSystem()
    : BaseSystem()
{       
    ecs::World::GetInstance().SetSingletonComponent<DebugViewStateSingletonComponent>(std::make_unique<DebugViewStateSingletonComponent>());        
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::VUpdate(const float, const std::vector<ecs::EntityId>&) const
{
    HandleFrameStatsDisplay();
    HandleLightsDebug();
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::HandleFrameStatsDisplay() const
{
    const auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();    
    
    if (debugViewStateComponent.mFrameStatsDisplayEnabled)
    {
        // Update text only once per second (once the fps counter changes)
        if (debugViewStateComponent.mFpsStrings.second == ecs::NULL_ENTITY_ID ||
            !rendering::IsTextStringTheSameAsText
        (
            debugViewStateComponent.mFpsStrings.second, 
            std::to_string(debugViewStateComponent.mCurrentFps)
        ))
        {
            RenderFpsString();
            RenderEntityCountString();
            RenderSystemUpdateStrings();
        }                   
    }
    else
    {
        ClearFrameStatsStrings();
    }
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::HandleLightsDebug() const
{
    auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();
    
    const auto& lightStoreComponent = world.GetSingletonComponent<rendering::LightStoreSingletonComponent>();
    
    if (debugViewStateComponent.mLightDebugDisplayEnabled)
    {
        if (debugViewStateComponent.mDebugLightEntities.size() != lightStoreComponent.mLightPositions.size())
        {
            ClearDebugLights();
            CreateDebugLights();
        }
        
        UpdateDebugLightsPosition();
    }
    else if (debugViewStateComponent.mDebugLightEntities.size() != 0)
    {
        ClearDebugLights();
    }
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::ClearFrameStatsStrings() const
{
    const auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();

    if (debugViewStateComponent.mFpsStrings.first != ecs::NULL_ENTITY_ID)
    {
        rendering::DestroyRenderedText(debugViewStateComponent.mFpsStrings.first);
        rendering::DestroyRenderedText(debugViewStateComponent.mFpsStrings.second);

        debugViewStateComponent.mFpsStrings.first  = ecs::NULL_ENTITY_ID;
        debugViewStateComponent.mFpsStrings.second = ecs::NULL_ENTITY_ID;
    }

    if (debugViewStateComponent.mEntityCountStrings.first != ecs::NULL_ENTITY_ID)
    {
        rendering::DestroyRenderedText(debugViewStateComponent.mEntityCountStrings.first);
        rendering::DestroyRenderedText(debugViewStateComponent.mEntityCountStrings.second);

        debugViewStateComponent.mEntityCountStrings.first = ecs::NULL_ENTITY_ID;
        debugViewStateComponent.mEntityCountStrings.second = ecs::NULL_ENTITY_ID;
    }

    if (debugViewStateComponent.mSystemNamesAndUpdateTimeStrings.size() > 0)
    {
        for (const auto& systemNameAndUpdateTimeStrings : debugViewStateComponent.mSystemNamesAndUpdateTimeStrings)
        {
            rendering::DestroyRenderedText(systemNameAndUpdateTimeStrings.first);
            rendering::DestroyRenderedText(systemNameAndUpdateTimeStrings.second);
        }

        debugViewStateComponent.mSystemNamesAndUpdateTimeStrings.clear();
    }
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::ClearDebugLights() const
{
    auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();
    
    world.DestroyEntities(debugViewStateComponent.mDebugLightEntities);
    debugViewStateComponent.mDebugLightEntities.clear();
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::RenderFpsString() const
{
    const auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();

    auto fpsTextColor = rendering::colors::BLACK;
    if (debugViewStateComponent.mCurrentFps < 50)
    {
        fpsTextColor = rendering::colors::YELLOW;
    }
    if (debugViewStateComponent.mCurrentFps < 20)
    {
        fpsTextColor = rendering::colors::RED;
    }
    
    debugViewStateComponent.mFpsStrings.first = rendering::RenderTextIfDifferentToPreviousString
    (
        "FPS: ",
        debugViewStateComponent.mFpsStrings.first,
        TEXT_FONT_NAME,
        TEXT_SIZE,
        FPS_TEXT_POSITION,
        fpsTextColor
    );   
    
    debugViewStateComponent.mFpsStrings.second = rendering::RenderTextIfDifferentToPreviousString
    (
        std::to_string(debugViewStateComponent.mCurrentFps),
        debugViewStateComponent.mFpsStrings.second,
        TEXT_FONT_NAME,
        TEXT_SIZE,
        FPS_NUMBER_POSITION,
        fpsTextColor
    );            
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::RenderEntityCountString() const
{
    const auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();

    debugViewStateComponent.mEntityCountStrings.first = rendering::RenderTextIfDifferentToPreviousString
    (
        "Entities: ",
        debugViewStateComponent.mEntityCountStrings.first,
        TEXT_FONT_NAME,
        TEXT_SIZE,
        ENTITY_COUNT_TEXT_POSITION,
        rendering::colors::BLACK
    );

    debugViewStateComponent.mEntityCountStrings.second = rendering::RenderTextIfDifferentToPreviousString
    (
        std::to_string(world.GetEntityCount()),
        debugViewStateComponent.mEntityCountStrings.second,
        TEXT_FONT_NAME,
        TEXT_SIZE,
        ENTITY_COUNT_NUMBER_POSITION,
        rendering::colors::BLACK
    );
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::RenderSystemUpdateStrings() const
{
    const auto& world = ecs::World::GetInstance();
    const auto& systemUpdateTimes = world.GetSystemUpdateTimes();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();

    const auto isFirstRendering = debugViewStateComponent.mSystemNamesAndUpdateTimeStrings.size() == 0;

    auto systemCounter = 0;
    for (const auto& systemNameUpdateTimePair: systemUpdateTimes)
    {
        auto previousSystemNameString       = ecs::NULL_ENTITY_ID;
        auto previousSystemUpdateTimeString = ecs::NULL_ENTITY_ID;

        if (!isFirstRendering)
        {
            previousSystemNameString       = debugViewStateComponent.mSystemNamesAndUpdateTimeStrings[systemCounter].first;
            previousSystemUpdateTimeString = debugViewStateComponent.mSystemNamesAndUpdateTimeStrings[systemCounter].second;
        }

        auto systemNamePosition = SYSTEM_NAMES_STARTING_POSITION;
        systemNamePosition.y -= systemCounter * TEXT_SIZE;

        auto systemNameString = rendering::RenderTextIfDifferentToPreviousString
        (
            systemNameUpdateTimePair.first.GetString(),
            previousSystemNameString,
            TEXT_FONT_NAME,
            TEXT_SIZE,
            systemNamePosition,
            rendering::colors::BLACK
        );

        auto systemUpdateTimePosition = SYSTEM_UPDATE_TIME_STARTING_POSITION;
        systemUpdateTimePosition.y -= systemCounter * TEXT_SIZE;

        auto systemUpdateTimeString = rendering::RenderTextIfDifferentToPreviousString
        (
            std::to_string(systemNameUpdateTimePair.second) + " micros",
            previousSystemUpdateTimeString,
            TEXT_FONT_NAME,
            TEXT_SIZE,
            systemUpdateTimePosition,
            rendering::colors::BLACK
        );

        if (isFirstRendering)
        {
            debugViewStateComponent.mSystemNamesAndUpdateTimeStrings.push_back({ systemNameString, systemUpdateTimeString });           
        }
        else
        {
            debugViewStateComponent.mSystemNamesAndUpdateTimeStrings[systemCounter] = std::make_pair(systemNameString, systemUpdateTimeString);
        }
        
        systemCounter++;        
    }
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::CreateDebugLights() const
{
    auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();
    const auto& lightStoreComponent = world.GetSingletonComponent<rendering::LightStoreSingletonComponent>();
    
    for (auto i = 0U; i < lightStoreComponent.mLightPositions.size(); ++i)
    {
        auto cubeEntity = rendering::LoadAndCreateModelByName
        (
            DEBUG_LIGHT_ASSET_NAME.GetString(),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            DEBUG_LIGHT_SCALE,
            DEBUG_LIGHT_ASSET_NAME
        );
        
        auto& renderableComponent = world.GetComponent<rendering::RenderableComponent>(cubeEntity);
        renderableComponent.mMaterial.mAmbient   = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
        renderableComponent.mMaterial.mDiffuse   = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        renderableComponent.mMaterial.mSpecular  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        renderableComponent.mMaterial.mShininess = 32.0f;
        
        debugViewStateComponent.mDebugLightEntities.push_back(cubeEntity);
    }
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::UpdateDebugLightsPosition() const
{
    auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();
    const auto& lightStoreComponent = world.GetSingletonComponent<rendering::LightStoreSingletonComponent>();
    
    for (auto i = 0U; i < lightStoreComponent.mLightPositions.size(); ++i)
    {
        auto& debugLightPosition = world.GetComponent<TransformComponent>(debugViewStateComponent.mDebugLightEntities[i]);
        debugLightPosition.mPosition = lightStoreComponent.mLightPositions[i];
    }
}

///-----------------------------------------------------------------------------------------------

}

}
