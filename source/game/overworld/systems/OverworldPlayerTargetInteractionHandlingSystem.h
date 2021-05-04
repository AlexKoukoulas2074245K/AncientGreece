///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetInteractionHandlingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 10/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldPlayerTargetInteractionHandlingSystem_h
#define OverworldPlayerTargetInteractionHandlingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldInteractionComponent;

///-----------------------------------------------------------------------------------------------
class OverworldPlayerTargetInteractionHandlingSystem final : public genesis::ecs::BaseSystem<OverworldInteractionComponent>
{
public:
    OverworldPlayerTargetInteractionHandlingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

private:
    void ShowUnitInteractionView(const OverworldInteractionComponent&) const;
    void ShowOngoingBattleUnitIntractionView(const genesis::ecs::EntityId battleStateEntity) const;
    void ShowCityStateInteractionView(const OverworldInteractionComponent&) const;
    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldPlayerTargetInteractionHandlingSystem_h */
