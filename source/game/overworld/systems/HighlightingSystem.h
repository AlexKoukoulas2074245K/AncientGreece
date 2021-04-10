///------------------------------------------------------------------------------------------------
///  HighlightingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef HighlightingSystem_h
#define HighlightingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class HighlightableComponent;

///-----------------------------------------------------------------------------------------------
class HighlightingSystem final : public genesis::ecs::BaseSystem<HighlightableComponent>
{
public:
    HighlightingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void CreateUnitPreviewPopup(const glm::vec3& unitPosition, const UnitStatsComponent& unitStatsComponent) const;
    void CreateCityStatePreviewPopup(const glm::vec3& cityStatePosition, const StringId& cityStateName) const;
    void DestroyUnitPreviewPopup() const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* HighlightingSystem_h */
