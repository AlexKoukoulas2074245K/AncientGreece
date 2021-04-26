///------------------------------------------------------------------------------------------------
///  OverworldDayTimeUpdaterSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 18/04/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldDayTimeUpdaterSystem.h"
#include "../components/OverworldDayTimeSingletonComponent.h"
#include "../utils/OverworldDayTimeUtils.h"
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/LightStoreSingletonComponent.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const int INIT_YEAR_BC = 500;
    static const int INIT_DAY     = 1;
    static const int DAYS_IN_YEAR = 365;

    static const float HOURS_IN_DAY            = 24.0f;
    static const float TIME_STRING_SIZE        = 0.1f;
    static const float CURRENT_PERIOD_Y_OFFSET = -0.94f;
    
    static const std::string TIME_DISPLAY_BACKGROUND_SPRITE_MODEL   = "gui_base";
    static const std::string TIME_DISPLAY_BACKGROUND_SPRITE_TEXTURE = "horizontal_parchment";

    static const StringId DAWN_PERIOD_NAME          = StringId("Dawn");
    static const StringId EARLY_MORNING_PERIOD_NAME = StringId("Early Morning");
    static const StringId MORNING_PERIOD_NAME       = StringId("Morning");
    static const StringId MIDDAY_PERIOD_NAME        = StringId("Midday");
    static const StringId AFTERNOON_PERIOD_NAME     = StringId("Afternoon");
    static const StringId DUSK_PERIOD_NAME          = StringId("Dusk");
    static const StringId NIGHT_PERIOD_NAME         = StringId("Night");
    static const StringId MIDNIGHT_PERIOD_NAME      = StringId("Midnight");
    static const StringId GAME_FONT_NAME            = StringId("game_font");
    static const StringId TIME_DISPLAY_ENTITY_NAME  = StringId("time_display");
    static const StringId TIME_DISPLAY_SHADER_NAME  = StringId("default_gui");

    static const glm::vec3 TIME_DISPLAY_BACKGROUND_POSITION = glm::vec3(-0.69f, -0.89f, 0.01f);
    static const glm::vec3 TIME_DISPLAY_BACKGROUND_SCALE    = glm::vec3(1.1f, 0.3f, 1.0f);
    static const glm::vec3 DAY_STRING_POSITION              = glm::vec3(-0.91f, -0.84f, 0.0f);
    static const glm::vec3 YEAR_STRING_POSITION             = glm::vec3(-0.63f, -0.84f, 0.0f);

    static const tsl::robin_map<StringId, float, StringIdHasher> PERIOD_NAME_TO_X_OFFSET =
    {
        { DAWN_PERIOD_NAME, -0.75f },
        { EARLY_MORNING_PERIOD_NAME, -0.89f },
        { MORNING_PERIOD_NAME, -0.79f },
        { MIDDAY_PERIOD_NAME, -0.79f },
        { AFTERNOON_PERIOD_NAME, -0.83f },
        { DUSK_PERIOD_NAME, -0.75f },
        { NIGHT_PERIOD_NAME, -0.76f },
        { MIDNIGHT_PERIOD_NAME, -0.80f }
    };
}

///-----------------------------------------------------------------------------------------------

OverworldDayTimeUpdaterSystem::OverworldDayTimeUpdaterSystem()
    : BaseSystem()
{
    auto dayTimeComponent = std::make_unique<OverworldDayTimeSingletonComponent>();
    dayTimeComponent->mCurrentDay = INIT_DAY;
    dayTimeComponent->mCurrentYearBc = INIT_YEAR_BC;
    dayTimeComponent->mCurrentPeriod = StringId();
    dayTimeComponent->mTimeDtAccum = 0.0f;
    
    genesis::ecs::World::GetInstance().SetSingletonComponent<OverworldDayTimeSingletonComponent>(std::move(dayTimeComponent));
}

///-----------------------------------------------------------------------------------------------

void OverworldDayTimeUpdaterSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& dayTimeComponent = world.GetSingletonComponent<OverworldDayTimeSingletonComponent>();
    auto& lightStoreComponent = world.GetSingletonComponent<genesis::rendering::LightStoreSingletonComponent>();
    
    // Calculate current day time timestamp
    dayTimeComponent.mTimeDtAccum += GetDayTimeSpeed() * dt;
    dayTimeComponent.mTimeDtAccum = std::fmod(dayTimeComponent.mTimeDtAccum, genesis::math::PI * 2.0f);
    
    // Position "sun" depending on current timestamp
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(dayTimeComponent.mTimeDtAccum);
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(dayTimeComponent.mTimeDtAccum);
    
    // Get ration to 24 hours
    const auto dayCycleFactor = (dayTimeComponent.mTimeDtAccum/(genesis::math::PI * 2.0f)) * HOURS_IN_DAY;
    
    // Decide on day period
    const auto previousPeriod = dayTimeComponent.mCurrentPeriod;
    dayTimeComponent.mCurrentPeriod = NIGHT_PERIOD_NAME;
    if (dayCycleFactor <= 3.0f) dayTimeComponent.mCurrentPeriod = MIDNIGHT_PERIOD_NAME;
    else if (dayCycleFactor <= 6.0f) dayTimeComponent.mCurrentPeriod = DAWN_PERIOD_NAME;
    else if (dayCycleFactor <= 9.0f) dayTimeComponent.mCurrentPeriod = EARLY_MORNING_PERIOD_NAME;
    else if (dayCycleFactor <= 12.0f) dayTimeComponent.mCurrentPeriod = MORNING_PERIOD_NAME;
    else if (dayCycleFactor <= 15.0f) dayTimeComponent.mCurrentPeriod = MIDDAY_PERIOD_NAME;
    else if (dayCycleFactor <= 18.0f) dayTimeComponent.mCurrentPeriod = AFTERNOON_PERIOD_NAME;
    else if (dayCycleFactor <= 21.0f) dayTimeComponent.mCurrentPeriod = DUSK_PERIOD_NAME;
    else if (dayCycleFactor <= 24.0f) dayTimeComponent.mCurrentPeriod = NIGHT_PERIOD_NAME;
    
    // Period change
    dayTimeComponent.mPeriodChangeTick = false;
    dayTimeComponent.mDayChangeTick    = false;
    dayTimeComponent.mYearChangeTick   = false;
    
    if (previousPeriod != dayTimeComponent.mCurrentPeriod)
    {
        dayTimeComponent.mPeriodChangeTick = true;
        
        // Day change
        if (previousPeriod == NIGHT_PERIOD_NAME && dayTimeComponent.mCurrentPeriod == MIDNIGHT_PERIOD_NAME)
        {
            dayTimeComponent.mDayChangeTick = true;
            dayTimeComponent.mCurrentDay++;
            if (dayTimeComponent.mCurrentDay > DAYS_IN_YEAR)
            {
                dayTimeComponent.mYearChangeTick = true;
                dayTimeComponent.mCurrentDay = INIT_DAY;
                dayTimeComponent.mCurrentYearBc--;
            }
        }
    }
    
    world.DestroyEntities(world.FindAllEntitiesWithName(TIME_DISPLAY_ENTITY_NAME));
    
    // Background Sprite
    genesis::rendering::LoadAndCreateGuiSprite(TIME_DISPLAY_BACKGROUND_SPRITE_MODEL, TIME_DISPLAY_BACKGROUND_SPRITE_TEXTURE, TIME_DISPLAY_SHADER_NAME,  TIME_DISPLAY_BACKGROUND_POSITION, glm::vec3(), TIME_DISPLAY_BACKGROUND_SCALE, false, TIME_DISPLAY_ENTITY_NAME);
    
    // Day Display
    genesis::rendering::RenderText("Day " + std::to_string(dayTimeComponent.mCurrentDay), GAME_FONT_NAME, TIME_STRING_SIZE, DAY_STRING_POSITION, genesis::colors::BLACK, false, TIME_DISPLAY_ENTITY_NAME);
    
    // Year Display
    genesis::rendering::RenderText(std::to_string(dayTimeComponent.mCurrentYearBc) + "bc", GAME_FONT_NAME, TIME_STRING_SIZE, YEAR_STRING_POSITION, genesis::colors::BLACK, false, TIME_DISPLAY_ENTITY_NAME);
    
    // Current period Display
    genesis::rendering::RenderText(dayTimeComponent.mCurrentPeriod.GetString(), GAME_FONT_NAME, TIME_STRING_SIZE, glm::vec3(PERIOD_NAME_TO_X_OFFSET.at(dayTimeComponent.mCurrentPeriod), CURRENT_PERIOD_Y_OFFSET, 0.0f), genesis::colors::BLACK, false, TIME_DISPLAY_ENTITY_NAME);
}

///-----------------------------------------------------------------------------------------------

}
