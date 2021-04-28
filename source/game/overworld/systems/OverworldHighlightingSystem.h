///------------------------------------------------------------------------------------------------
///  OverworldHighlightingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldHighlightingSystem_h
#define OverworldHighlightingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldHighlightableComponent;

///-----------------------------------------------------------------------------------------------
class OverworldHighlightingSystem final : public genesis::ecs::BaseSystem<OverworldHighlightableComponent>
{
public:
    OverworldHighlightingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void CreateUnitPreviewPopup(const glm::vec3& unitPosition, const StringId& behaviourDescription, const UnitStatsComponent& unitStatsComponent) const;
    void CreateCityStatePreviewPopup(const glm::vec3& cityStatePosition, const StringId& cityStateName) const;
    void DestroyUnitPreviewPopup() const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldHighlightingSystem_h */
