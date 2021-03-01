///------------------------------------------------------------------------------------------------
///  DebugViewManagementSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef DebugViewManagementSystem_h
#define DebugViewManagementSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------
class DebugViewManagementSystem final : public ecs::BaseSystem<ecs::NullComponent>
{
public:
    DebugViewManagementSystem();

    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;

private:
    void HandleFrameStatsDisplay() const;
    void HandleLightsDebug() const;
    void ClearFrameStatsStrings() const;
    void ClearDebugLights() const;
    void RenderFpsString() const;
    void RenderEntityCountString() const;
    void RenderSystemUpdateStrings() const;
    void CreateDebugLights() const;
    void UpdateDebugLightsPosition() const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* DebugViewManagementSystem_h */
